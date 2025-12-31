#include "EventLogger.h"
#include "ActiveThreatsManager.h"
#include <cmath>
#include <unistd.h>
#include <stdexcept>
// #include <Eigen/Dense> // already included in "ActiveThreatsManager.h"

#define InSymmetricRange(value, symmetric_bound) (value <= symmetric_bound && value >= -symmetric_bound)

#define SafeSumAngle(a, b) (a + b > 180.0 ? a + b - 360.0 : a + b < -180.0 ? 360.0 + a + b : a + b)

ActiveThreatsManager::Signature::Signature(uint64_t threat_unique_id, uint16_t probe_id, const encoderThreat & threat_descriptor):
    encoderThreat(threat_descriptor),
    threat_id(threat_unique_id),
    probe_id(probe_id)
{

}

uint64_t ActiveThreatsManager::unique_threat_id_counter = 1;
uint64_t ActiveThreatsManager::createNewThreatId()
{
    return unique_threat_id_counter++;
}

/*!
*@brief Converts WGS84 coordinates to ECEF
*@param lat latitude in WGS84 format
*@param lon longitude in WGS84 format
*@param h height(altitude) in meters
*@return std::vector containing x, y and z values in ECEF format.
*/
std::vector<double> WGS84ToECEF(double lat, double lon, double h)
{
    std::vector<double> res = {};
    
    double a = 6378137.0;
    double b = 6356752.314245;
    double f = (a-b)/a;
    double f_inv = 1.0 / f;

    double a_sq = pow(a,2);
    double b_sq = pow(a,2);
    double e_sq = f * (2-f);

    double n = a / sqrt(1-e_sq * pow(sin(lat*(M_PI/180.0)),2));

    res.push_back((h+n) * cos(lat*(M_PI/180.0)) * cos(lon*(M_PI/180.0)));
    res.push_back((h+n) * cos(lat*(M_PI/180.0)) * sin(lon*(M_PI/180.0)));
    res.push_back((h +(1-e_sq) * n) * sin(lat*(M_PI/180.0)));

    return res;
}

/*!
*@brief Converts ECEF coordinates to WGS84
*@param x axis in ECEF coordinate system
*@param y axis in ECEF coordinate system
*@param z axis in ECEF coordinate system
*@return std::vector containing latitude, longitude and altitude in WGS84 format.
*/
std::vector<double> ECEFToWGS84(double x, double y,double z)
{
    std::vector<double> res = {};
    
    double a = 6378137.0;
    double b = 6356752.314245;
    double f = (a-b)/a;
    double f_inv = 1.0 / f;

    double a_sq = pow(a,2);
    double b_sq = pow(b,2);
    double e_sq = f * (2-f);

    double eps = e_sq / (1.0 - e_sq);
    double p = sqrt(x*x + y*y);
    double q = atan2((z*a), (p*b));

    double rlat = atan2((z+eps*b*pow(sin(q),3)), (p - e_sq * a * pow(cos(q),3)));
    double rlon = atan2(y,x);
    double v = a / sqrt(1.0 - e_sq * sin(rlat) * sin(rlat));
    double h = (p / cos(rlat)) - v;

    res.push_back(rlat*(180.0/M_PI));
    res.push_back(rlon*(180.0/M_PI));
    res.push_back(h);
    return res;
}

/*!
*@brief Gets proper earth radius for given latitude
*@param lat latitude for which radius is being calculated for
*@return double value containing earth radius on given latitude in meters.
*/
double EarthRadiusAtLatitude(double lat)
{
    double rlat = lat*(M_PI/180.0);

    double a = 6378137.0;
    double b = 6356752.3;

    double r = sqrt((pow((a*a*cos(rlat)), 2) + pow((b*b*sin(rlat)), 2)) / (pow((a*cos(rlat)), 2) + (pow((b*sin(rlat)),2))));
    return r;
}

/*!
* @brief Updates threat geolocation based on received signatures.
* @details Loops through all signatures available, when 6 signatures are stored (one from each probe)
* it will attempt to calculate position based on received signal strengths and probe locations.
* Calculations of distance based on signal strength are based on probe simulation code (should be changed).
* After distance calculation WGS84 cooridnates are converted to ECEF for equation system solving.
* Each signature is then used to create equation system stored in matrix from Eigen library.
* Location is calculated with least squares method. Calculated position is converted back to WGS84
* @param pThreat threat for which geolocation update is calculated.
* @param probe_unique_id unique identifier of probe from which new signature was received.
* @param threat_descriptor descriptor of threat
* @returns Nothing
*/
void ActiveThreatsManager::updateGeolocation(ActiveThreatsManager::Threat &pThreat, uint32_t probe_unique_id, const ActiveThreatsManager::encoderThreat &threat_descriptor) throw(std::range_error)
{
    { // critical segment
        std::unique_lock<std::mutex> lock(this->signatures_queue_mutex); // lock instance created, but lock isn't applied!
        auto bucket = this->getBucket(threat_descriptor.getType());
        auto signList = (*bucket)[pThreat.getUniqueId()].signatures;

        //For now wait for all probes (testing), value 6 is hardcoded
        if(signList.size() < 6) 
        {
            return;
        }

        std::vector<double> cx;
        std::vector<double> cy;
        std::vector<double> cz;
        std::vector<double> cd;

        for(auto signature : signList)
        {
            //get received signal power
            double signalP = (double)(signature.second->sygn_J * 0.008);
            //part that should be changed
            //Convert received signal strength to distance
            //This should be changed to use other way to obtain distance or position.
            //It was made only for simulation purposes and won't work in real use case.
            double sPow = pow(10.0, ((-69.008 - signalP)/10.0));  // OPTIMIZE:
            double distance = sqrt(sPow/(4*M_PI));
            //end of part that should be changed

            //Get sginature probe location
            // XXX:
            ActiveThreatsManager::GeolocationData probeLocation = this->getProbePosition(signature.second->getProbeId());
            //If probe position is valid push values to arrays.
            if(probeLocation.isValid())
            {
                //Convert WGS84 coordinates of probe to ECEF
                auto conv = WGS84ToECEF(probeLocation.latitude, probeLocation.longitude, 0.0);
                cx.push_back(conv[0]);
                cy.push_back(conv[1]);
                cz.push_back(conv[2]);
                cd.push_back(distance);
            }
        }

        //Get amount of used signatures.
        size_t count = cd.size();
        //Multilateration with unique point requires at least 4 valid signatures with using ECEF.
        // ... requires 4 unique 'non-degenerated' geographical points, better having at least 6
        if(count < 3) return;

        //Clear already used signatures (testing)
        (*bucket)[pThreat.getUniqueId()].signatures.clear();

        Eigen::MatrixXd m(count,3);
        Eigen::VectorXd c(count);
        for(size_t i = 0; i < count; i++)
        {
            m(i,0) = 2*cx[i];
            m(i,1) = 2*cy[i];
            m(i,2) = 2*cz[i];
            //6371000 is mean earth radius in meters.
            c(i) = 6371000*6371000 + (cx[i]*cx[i] + cy[i]*cy[i] + cz[i]*cz[i]) - cd[i]*cd[i];  // XXX:
        }

        //Solve equation system using least squares method
        auto mt = m.transpose();
        auto mtm = mt*m;
        auto mtm_inv = mtm.inverse();
        auto mplus = mtm_inv * mt;
        auto location = mplus*c;

        //Convert ECEF back to WGS84
        auto out = ECEFToWGS84(location[0], location[1], location[2]);
        //Update threat position to newly calculated position
        pThreat.position.latitude = out[0];
        pThreat.position.longitude = out[1];

    }
}

// void ActiveThreatsManager::updateGeolocation2(Threat &pThreat, const encoderThreat & threat_description)
// {
//     { // critical segment
//         std::unique_lock<std::mutex> lock(this->signatures_queue_mutex); // lock instance created, but lock isn't applied!
//         auto bucket = this->getBucket(threat_descriptor.getType());
//         auto signList = (*bucket)[pThreat.getUniqueId()].signatures;

//         //For now wait for all probes (testing), value 6 is hardcoded
//         if(signList.size() < 6) 
//         {
//             return; // actually, estimating geolocation makes no sense for small number of probes; leaving this
//         }

//         std::vector<std::function<Eigen::Vector2d>> storage;

//         for(uint16_t i = 0; i < signList.size() - 1; i++) // TODO: refactor using iterators
//         {
//             for(uint16_t j = i + 1; j < signList.size(); j++) // REVIEW: are unique signatures accessed?
//             {
//                 // At this point, i and j indexers provide pairs of signatures from two independant probes
//                 // Assuming that logarithmic-derived powers (dB, dBm) are provided - according to  "Komunikacja_zbiór_danych.pdf"
//                 double temp_power = signList[j].sygn_J - sign_list[i].sygn_J;
//                 // calculate distance difference for given power ratio
//                 double temp_dist = power2distance_logarithmic(temp_power, signList[j].f0 * 1000.0);
//                 // auto temp = get_hyperbolic_equation(signList[i])
//                 // storage.
//             }
//         }

//         for(auto signature : signList)
//         {
//             //get received signal to noise power ratio
//             // assuming that this value is sent in dBm...
//             double signalToNoise = static_cast<double>(signature.second->sygn_J) - static_cast<double>(signature.second->sygn_No);
//             // if not, use next expression:
//             // double signalToNoise = (static_cast<double>(signature.second->sygn_J)) / (static_cast<double>(signature.second->sygn_No));
            
//             //part that should be changed
//             //Convert received signal strength to distance
//             //This should be changed to use other way to obtain distance or position.
//             //It was made only for simulation purposes and won't work in real use case.
//             double sPow = pow(10.0, ((-69.008 - signalP)/10.0));
//             double distance = sqrt(sPow/(4*M_PI));
//             //end of part that should be changed

//             //Get sginature probe location
//             ActiveThreatsManager::GeolocationData probeLocation = this->getProbePosition(signature.second->getProbeId());
//             //If probe position is valid push values to arrays.
//             if(probeLocation.isValid())
//             {
//                 //Convert WGS84 coordinates of probe to ECEF
//                 auto conv = WGS84ToECEF(probeLocation.latitude, probeLocation.longitude, 0.0);

//             }
//         }
//     }
// }


std::pair<uint32_t, ActiveThreatsManager::encoderThreat> ActiveThreatsManager::pop()
{
    std::unique_lock<std::mutex> lock(this->signatures_queue_mutex);
    this->condition_var.wait(lock, [=]{ return !this->signatures_queue.empty(); });
    std::pair<uint32_t,encoderThreat> result(std::move(this->signatures_queue.back()));
    this->signatures_queue.pop_back();
    return result;
}

void ActiveThreatsManager::worker()
{
    while(is_worker_running) {
        auto signature = this->pop();
        // was forced to quit?
        if(!is_worker_running) break;
        // earse not active threats
        Watchdog::checkForExpiredThreats(&this->active_cache,5000);
        // classify threat
        auto classified_threat_id = this->threat_classifier.classify(signature.second, signature.first);
        // write signature to the ActiveThreatsManager (signature will be automatically pushed to the database after it expires)
        this->appendSignature(signature.first,classified_threat_id,signature.second);
    }
}

std::shared_ptr<ActiveThreatsManager::Threat> ActiveThreatsManager::getActiveThreat(ThreatClassifier::threat_type_t threat_type, uint64_t threat_unique_id)
{
    auto bucket = getBucket(threat_type);
    auto it = bucket->find(threat_unique_id);
    if (it != bucket->end()) {
        return it->second.pThreat;
    }
    return std::shared_ptr<Threat>(nullptr);
}


std::list<std::shared_ptr<ActiveThreatsManager::Threat> > ActiveThreatsManager::getActiveThreats(ThreatClassifier::threat_type_t threat_type, uint32_t probe_unique_id)
{
    auto bucket = getBucket(threat_type);
    std::list<std::shared_ptr<ActiveThreatsManager::Threat>> v;
    if(probe_unique_id == 0)
    {
        // geolocation filter not specified
        for(auto & item : *bucket) {
            if(nullptr != item.second.pThreat){
                v.push_back(item.second.pThreat);
            }
        }
    }else
    {
        // data should be filtered by region

        // get probe geolocation
        // for performance this should be also cached

        // list comprehension
        for(auto & item : *bucket)
        {
            // TODO: filter out threats by distance
            //if(is in range)
            //{
                v.push_back(item.second.pThreat);
            //}
        }
    }

    return v;
}

ActiveThreatsManager::GeolocationData invalid_geolocation;
const ActiveThreatsManager::GeolocationData &ActiveThreatsManager::getProbePosition(uint32_t probe_unique_id)
{
    if(probe_geolocation_cache.find(probe_unique_id) != probe_geolocation_cache.end())
    {
        return probe_geolocation_cache.at(probe_unique_id);
    }
    return invalid_geolocation;
}

ActiveThreatsManager::cache_item_t *ActiveThreatsManager::appendSignature(uint32_t probe_unique_id, uint64_t threat_unique_id, const ActiveThreatsManager::encoderThreat &threat_descriptor)
{
    // TODO: add here threat and signature push to database

    auto bucket = this->getBucket(threat_descriptor.getType());

    // try to pick matching threat
    auto pThreat = getActiveThreat(threat_descriptor.getType(), threat_unique_id);

    if(threat_unique_id == 0 || pThreat == nullptr)
    {
        // The associated threat in cache was not found for this signature
        // create new threat
        threat_unique_id = ActiveThreatsManager::createNewThreatId();
        auto & item = (*bucket)[threat_unique_id];
        item.pThreat = std::shared_ptr<Threat>(new Threat(threat_unique_id,threat_descriptor,this->time_utils));
        // push new signature to actives cache
        item.signatures[probe_unique_id] = std::shared_ptr<Signature>(new Signature(threat_unique_id,probe_unique_id,threat_descriptor));

        return &item;
    }
    else
    {
        // The associated theat has been found for this signature
        pThreat->model.update(threat_descriptor);
        // push new signature to actives cache
        (*bucket)[threat_unique_id].signatures[probe_unique_id] = std::shared_ptr<Signature>(new Signature(threat_unique_id,probe_unique_id,threat_descriptor));
        //Update position of threat
        this->updateGeolocation(*pThreat, probe_unique_id, threat_descriptor);
        return &(*bucket)[threat_unique_id];
    }

}

ActiveThreatsManager::bucket_t *ActiveThreatsManager::getBucket(ThreatClassifier::threat_type_t threat_type)
{
    return &this->active_cache.at(threat_type);
}

void ActiveThreatsManager::pushSignature(uint32_t probe_unique_id, const ActiveThreatsManager::encoderThreat &signature, double longitude, double latitude)
{
    { // critical segment
        std::unique_lock<std::mutex> lock(this->signatures_queue_mutex);
        this->signatures_queue.push_front(std::make_pair(probe_unique_id,signature));
        GeolocationData probeGeoloc;
        probeGeoloc.latitude = latitude;
        probeGeoloc.longitude = longitude;
        this->probe_geolocation_cache[probe_unique_id] = probeGeoloc;
    }
    this->condition_var.notify_one();
}

ActiveThreatsManager::ActiveThreatsManager(uint32_t active_threats_lifetime_ms) :
    active_cache(ThreatClassifier::threat_type_t::SIGNAL_TYPE_RESERVED),
    watchdog(nullptr),
    threat_classifier(this),
    dumpToFile(true)
{
    this->time_utils = new TimeUtils();
    if(active_threats_lifetime_ms != 0){
        // NOTE: potential multithreading issues when spawning Watchdog in different thread
        // to avoid this temporarly don't spawn watchdog in different thread and call static
        // checkForExpiredThreats manually in current thread
        // passing nullptr as first argument will prevent watchdog constructor from spawning thread for itself

        //watchdog = new Watchdog(&this->active_cache,this->time_utils,active_threats_lifetime_ms);
        watchdog = new Watchdog(nullptr,this->time_utils,active_threats_lifetime_ms);
    }
}

ActiveThreatsManager::~ActiveThreatsManager()
{
    if(this->watchdog != nullptr) delete this->watchdog;
    if(this->time_utils != nullptr) delete this->time_utils;
    if(this->worker_thread.joinable()){
        this->is_worker_running = false;
        // push dummy signature in order to unblock worker_thread ( its supposed to be suspended until new signature is present )
        // then it will be able to gracefully terminate
        encoderThreat t;
        this->pushSignature(1,t,0.0,0.0);
        this->worker_thread.join();
    }
}

void ActiveThreatsManager::start()
{
    this->is_worker_running = true;
    if(!this->worker_thread.joinable()) {
        this->worker_thread = std::thread(&ActiveThreatsManager::worker, this);
        this->worker_thread.detach();
    }
}


ActiveThreatsManager::Threat::Threat(uint64_t unique_id, const ThreatClassifier::threat_descriptor_t & threat, ITimeUtils *time_utils):
    unique_id(unique_id),
    model(unique_id,threat),
    time_utils(time_utils)
{
    creation_time = time_utils->getTimeNow();
    last_update_time = creation_time;
}

uint32_t ActiveThreatsManager::Threat::getLastUpdatePeriod() const
{
    return (time_utils->getTimeNow() - this->last_update_time).count();
}

const double ActiveThreatsManager::FriisEquationUtils::c = 299792458.0;

double ActiveThreatsManager::FriisEquationUtils::watts_to_dBW(double _power_abs)
{
    return log10(_power_abs);
}

double ActiveThreatsManager::FriisEquationUtils::watts_to_dBm(double _power_abs)
{
    // return log10(_power_abs * 1000.0)
    return log10(_power_abs) + 30.0;
}

double ActiveThreatsManager::FriisEquationUtils::dBW_to_watts(double _power_dBW)
{
    return pow(10.0, _power_dBW / 10.0);
}

double ActiveThreatsManager::FriisEquationUtils::dBm_to_watts(double _power_dBm)
{
    return pow(10.0, (_power_dBm - 30.0) / 10.0);
}

double ActiveThreatsManager::FriisEquationUtils::estimate_rx_power(double _tx_power, double _frequency, double _distance, \
            double _tx_gain, double _rx_gain, double _ambient_gain, bool _decibels)
{
    double temp;
 
    if(_decibels)
    {
        // throw exceptions here
        if(_tx_gain == -1.0 && _rx_gain == -1.0 && _ambient_gain == -1.0)  // NOTE: exact-value floating point comparsion
            // REVIEW: Most probably, the default parameter values are provided. 
            _tx_gain = _rx_gain = _ambient_gain = 0.0;  // 0 is no damping/amplifying in dBi
        
        temp = 20.0 * log10(ActiveThreatsManager::FriisEquationUtils::c / (_frequency * 4.0 * M_PI * _distance));
        return _tx_power + _tx_gain + _rx_gain + _ambient_gain + temp;
    }
    else
    {
        // throw exceptions here
        if(_tx_gain == -1.0 && _rx_gain == -1.0 && _ambient_gain == -1.0)
            _tx_gain = _rx_gain = _ambient_gain = 1.0;  // 1 is the ratio between the same output and input signal strengths
        
        temp = (ActiveThreatsManager::FriisEquationUtils::c / (_frequency * 4.0 * M_PI * _distance));
        // temp = pow(temp, 2);
        temp *= temp;
        return _tx_power * _tx_gain * _rx_gain * _ambient_gain * temp;
    }
}

double ActiveThreatsManager::FriisEquationUtils::estimate_tx_power(double _rx_power, double _frequency, double _distance, \
            double _tx_gain, double _rx_gain, double _ambient_gain, bool _decibels)
{
    double temp;

    if(_decibels)
    {
        // throw exceptions here
        if(_tx_gain == -1.0 && _rx_gain == -1.0 && _ambient_gain == -1.0)  // NOTE: exact-value floating point comparsion
            // REVIEW: Most probably, the default parameter values are provided. 
            _tx_gain = _rx_gain = _ambient_gain = 0.0;  // 0 is no damping/amplifying in dBi
        
        temp = 20.0 * log10(ActiveThreatsManager::FriisEquationUtils::c / (_frequency * 4.0 * M_PI * _distance));
        return _rx_power - _tx_gain - _rx_gain - _ambient_gain - temp;
    }
    else
    {
        // throw exceptions here
        if(_tx_gain == -1.0 && _rx_gain == -1.0 && _ambient_gain == -1.0)
            _tx_gain = _rx_gain = _ambient_gain = 1.0;  // 1 is the ratio between the same output and input signal strengths
        
        temp = (ActiveThreatsManager::FriisEquationUtils::c / (_frequency * 4.0 * M_PI * _distance));
        // temp = pow(temp, 2);
        temp *= temp;
        return _rx_power / (_tx_gain * _rx_gain * _ambient_gain * temp);
    }
}

double ActiveThreatsManager::FriisEquationUtils::estimate_distance(double _tx_power, double _rx_power, double _frequency, \
            double _tx_gain, double _rx_gain, double _ambient_gain, bool _decibels)
{
    double temp;

    if(_decibels)
    {
        // throw exceptions here
        if(_tx_gain == -1.0 && _rx_gain == -1.0 && _ambient_gain == -1.0)  // NOTE: exact-value floating point comparsion
            // REVIEW: Most probably, the default parameter values are provided. 
            _tx_gain = _rx_gain = _ambient_gain = 0.0;  // 0 is no damping/amplifying in dBi
        
        temp = 20.0 * log10(ActiveThreatsManager::FriisEquationUtils::c / (4.0 * M_PI * _frequency));
        temp += _tx_power - _rx_power + _tx_gain + _rx_gain + _ambient_gain;

        return pow(10.0, temp / 20.0);
    }
    else
    {
        // throw exceptions here
        if(_tx_gain == -1.0 && _rx_gain == -1.0 && _ambient_gain == -1.0)
            _tx_gain = _rx_gain = _ambient_gain = 1.0;  // 1 is the ratio between the same output and input signal strengths

        temp = sqrt(_tx_power / _rx_power * _tx_gain * _rx_gain * _ambient_gain);
        return temp * ActiveThreatsManager::FriisEquationUtils::c / (4.0 * M_PI * _frequency);
    }
}

void ActiveThreatsManager::FriisEquationUtils::estimate_power_bounds(double _jtn_1, double _jtn_2, double _frequency, double _distance_between_probes, \
            double* _low_bound, double* _high_bound, bool _decibels)
{
    double temp = _jtn_2 - _jtn_1;
    

    // throw exceptions here
    if(temp > 0.0)  // just omit the 0 case and check for NaN's later
    {
        // no power_ratio signum reverting is required --- proceed as-is

        double distance_ratio = pow(10, temp / 20);  // d1/d2 ratio

        double distance_2_low = _distance_between_probes / (distance_ratio + 1.0);
        double distance_2_high = _distance_between_probes / (distance_ratio - 1.0);
        // NOTE: See expression above - if _distance_ratio equals approximately 1:1, then the upper bound is infinite
        // NOTE: this is consistent with the logical approach to the problem

        *_low_bound = ActiveThreatsManager::FriisEquationUtils::estimate_tx_power(_jtn_2, _frequency, distance_2_low);
        *_high_bound = ActiveThreatsManager::FriisEquationUtils::estimate_rx_power(_jtn_2, _frequency, distance_2_high);
        
        return;
    }
    else  // TODO: else if (temp < 0.0)
    {
        // power_ratio signum reverting required in this case
        temp = -temp;
        double distance_ratio = pow(10, temp / 20);  // d1/d2 ratio

        double distance_1_low = _distance_between_probes / (distance_ratio + 1.0);
        double distance_1_high = _distance_between_probes / (distance_ratio - 1.0);
        
        *_low_bound = ActiveThreatsManager::FriisEquationUtils::estimate_tx_power(_jtn_1, _frequency, distance_1_low);
        *_high_bound = ActiveThreatsManager::FriisEquationUtils::estimate_rx_power(_jtn_1, _frequency, distance_1_high);
        
        return;
    }
}

ActiveThreatsManager::GeolocationData::GeolocationData():
    longitude(NAN),
    latitude(NAN)
{

}

bool ActiveThreatsManager::GeolocationData::isValid()
{
    return (this->is_nan(this->latitude) || this->is_nan(this->longitude)) ? false : true;
}

std::list<uint64_t> ActiveThreatsManager::Watchdog::getExpiredThreatsFromBucket(const bucket_t &bucket, uint32_t threat_life_period_ms)
{
    std::list<uint64_t> expired_threats;
    for(auto & threat : bucket)
    {
        // check if time has expired
        uint32_t time_since_last_update = threat.second.pThreat->getLastUpdatePeriod();
        if(time_since_last_update > threat_life_period_ms){
            // yes, put this threat id in deleting list
            expired_threats.emplace_back(threat.second.pThreat->getUniqueId());
        }
    }
    return expired_threats;
}

void ActiveThreatsManager::Watchdog::deleteThreatsFromBucket(bucket_t &bucket, std::list<uint64_t> &threats_to_delete)
{
    for(auto threat_id : threats_to_delete){
        bucket.erase(threat_id);
    }
}

void ActiveThreatsManager::Watchdog::checkForExpiredThreats(cache_map_t *cache, uint32_t threat_life_period_ms)
{
    for(auto & bucket : *cache){
        // 1) get list of threats to delete in this bucket
        auto threats_to_delete = getExpiredThreatsFromBucket(bucket,threat_life_period_ms);
        // TODO: 2) push expired threats and signatures to the database

        // 3) delete obsolote cache items
        deleteThreatsFromBucket(bucket,threats_to_delete);
    }
}

void ActiveThreatsManager::Watchdog::worker()
{
    this->is_running = true;
    while(this->is_running)
    {
        checkForExpiredThreats(this->cache,this->threat_life_period_ms);
        this->time_utils->sleep_us(100*1000);
    }
}

ActiveThreatsManager::Watchdog::Watchdog(cache_map_t *cache, ITimeUtils *time_utils, uint32_t threat_life_period) :
    cache(cache),
    threat_life_period_ms(threat_life_period),
    is_running(false),
    time_utils(time_utils)
{
    if(cache != nullptr) {
        this->watchdog_thread = std::thread(&ActiveThreatsManager::Watchdog::worker, this);
        this->watchdog_thread.detach();
    }

}

ActiveThreatsManager::Watchdog::~Watchdog()
{
    this->stop();
}

void ActiveThreatsManager::Watchdog::stop()
{
    this->is_running = false;
    if(this->watchdog_thread.joinable()){
        this->watchdog_thread.join();
    }
}

void ActiveThreatsManager::TimeUtils::sleep_us(uint64_t microseconds)
{
    usleep(microseconds);
}

std::chrono::milliseconds ActiveThreatsManager::TimeUtils::getTimeNow()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
}
