#pragma once

#ifndef ACTIVETHREATSMANAGER_H  // EXCLUDE:
#define ACTIVETHREATSMANAGER_H


#include <stdint.h>
#include <unordered_map>
#include <list>
#include <vector>
#include <map>
#include <memory>
#include <probeMessagesEncoder.h>
#include "ThreatClassifier.h"
#include <chrono>
#include <thread>
#include <deque>
#include <mutex>
#include <utility>
#include <condition_variable>

#include <functional>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include "gauss.h"

#include <iostream>

#include <exception>

using namespace std;

/*!
*@brief Assumed frequency used in simulated testing of calculating threat position.
*/
#define SimFreq 10
/*!
*@brief Assumed transmitter power used in simulated testing of calculating threat position.
*/
#define SimTXpow 40.0

/*!
 * @brief Manages active threats.
 * This class implements a cache for active threats and signatures that belongs to the active threats
 */
class ActiveThreatsManager
{
    friend class ActiveThreatsManagerTest;

    public:
        //TESTING
        bool dumpToFile;
        typedef ProbeMessagesEncoder::messageThreat::threat_descriptor encoderThreat; ///! Threat descriptor alias from ProbeMessagesEncoder
        /*!
         * @brief Interface for time related functionalities
         */
        class ITimeUtils {
            public:
                /*!
                 * @brief Suspends current thread for specified number of microseconds
                 * @param microseconds number of microseconds to suspend execution of the current thread
                 */
                virtual void sleep_us(uint64_t microseconds) = 0;
                /*!
                 * @brief Returns current time formatted in milliseconds since epoch
                 * @return milliseconds since epoch
                 */
                virtual std::chrono::milliseconds getTimeNow() = 0;
                virtual ~ITimeUtils() {}
        };
        /*!
         * @brief Helper class for time related functionalities
         */
        class TimeUtils : public ITimeUtils {
            public:
                /*!
                 * @brief Suspends current thread for specified number of microseconds
                 * @param microseconds number of microseconds to suspend execution of the current thread
                 */
                void sleep_us(uint64_t microseconds);
                /*!
                 * @brief Returns current time formatted in milliseconds since epoch
                 * @return milliseconds since epoch
                 */
                std::chrono::milliseconds getTimeNow();

        };
        /*!
         * @brief Extends probe signature with classified thread unique id
         */
        struct GeolocationData
        {
            friend class ActiveThreatsManagerTest;
            //! @brief Mean Earth Radius constant.
            constexpr static double __MER = 6371000.0;
            protected:
                //! @brief LONGitude Angular Length constant.
                static constexpr double __LongAL = GeolocationData::__MER * M_PI / 180.0;
                
                //! @brief LATitude Angular Length inlined function.
                //! @param __mean_lat mean latitude e.g. latitude of single point or
                //! a mean latitude of two, three or more points etc.
                //! @returns Latitude angular length expressed in meters per angular degree.
                static inline double __LatAL(double __mean_lat)
                {
                    return GeolocationData::__LongAL * cos(__mean_lat * M_PI / 180.0);
                }
                /*!
                 * @brief Checks if double has NaN ( not a number )value. This helper method is introduced since std::isnan is not a standard function
                 * and some compilers may fail to link this
                 * @param __x value to check
                 * @return true if value is NaN. Otherwise false
                 */
                static bool is_nan(double __x) { return __x != __x;}
            public:
                static const double c = 299792458.0;

                double latitude;  // latitude class attribute expressed in degrees
                // TODO: fill geolocation data structure
                double longitude;  // longitude class attribute expressed in degrees
                // TODO: make getters, change these variables attributes to private
                double height;  // WGS84 ellipsoidal height
                /*!
                 * @brief Default implicit constructor. Initializes invalid values
                 */

                GeolocationData();
                /*!
                 * @brief Checks if structure holds proper geolocation data
                 * @return True if data is valid and can be used. False if data was not set or is invalid
                 */
                bool isValid();

                /*!
                 * @brief Calculates coarse loxodromic distance between 'this' and 'that' points.
                 * Used loxodromic distance calculation algoithm is reasonably accurate at short distances i.e. <= ~20km.
                 * @param that point to which loxodromic distance should be calculated.
                 * @returns loxodromic distance expressed in meters.
                 */
                double loxo_dist_to(const GeolocationData &that);
                
                /*!
                 * @brief Returns coarse loxodromic heading from 'this' point to 'that' point.
                 * Used heading calculation algorithm is reasonably accurate at short distances i.e. <= ~20km.
                 * @param that point to which heading should be calculated, started from 'this' object.
                 * @return heading angle expressed in radians.
                 */
                static double loxo_heading_to(const GeolocationData &that);
                
                /*!
                 * @brief OStream operator for serializing GeographicalPoint instances.
                 * @param os Output stream reference.
                 * @param _coord GeographicalPoint instance to be serialized and sent to stream.
                 * @returns the same OStream object reference.
                 */
                friend ostream& operator <<(ostream& os, GeolocationData& that);
        };

        class Signature : public encoderThreat
        {
                uint64_t threat_id; ///< Unique threat id that this signature was classified to
                uint16_t probe_id; ///< Unique probe id of the device which sent signature

            public:
                /*!
                 * @brief Constructor
                 * @param threat_unique_id, a threat unique identifier that signature was classified to
                 * @param threat_descriptor, threat descriptor catched from probe threat message
                 */
                Signature(uint64_t threat_unique_id, uint16_t probe_id, const encoderThreat & threat_descriptor);
                /*!
                 * @brief Gets threat id associated with this signature
                 * @return id of the threat
                 */
                uint64_t getThreatId() const { return threat_id; }
                /*!
                 * @brief Gets probe identifiers from which this signature was sent
                 * @return id of the probe
                 */
                uint16_t getProbeId() const { return probe_id; }
        };

        /*!
         * @brief Database threat record
         */
        class Threat
        {

            std::chrono::milliseconds creation_time; ///< Threat creation time in milliseconds since epoch
            std::chrono::milliseconds last_update_time; ///< Last threat data update time in milliseconds since epoch
            uint64_t unique_id; ///< threat unique id in database
            ITimeUtils * time_utils; ///< Time related utilities

            public:

            GeolocationData position; ///< Holds goelocation informations of the threat
            ThreatClassifier::ThreatModel model; ///< Bayesian classifier model of the threat
            gauss_t eirp;
            /*!
             * @brief Gets threat unique identifier
             * @return unique database id of this threat
             */
            const uint64_t& getUniqueId(){return unique_id;}  // REVIEW
            /*!
             * @brief Constructor
             * @param unique_id, a threat unique id
             * @param threat, threat descriptor that will be used to initialize model of the newly created threat
             * @param time_utils, pointer to the object containing time related utilities
             */
            Threat(uint64_t unique_id, const ThreatClassifier::threat_descriptor_t & threat, ITimeUtils * time_utils);
            /*!
             * @brief Gets threat life period in milliseconds i.e. the time between threat creation and the last threat model update time
             * @return Threat life period in milliseconds
             */
            uint32_t getLifePeriod() const {return (this->last_update_time - this->creation_time).count();}
            /*!
             * @brief Gets threat last update time in millisecods
             * @return Threat last update period in milliseconds
             */
            uint32_t getLastUpdatePeriod() const;
        };

        /*! @class FriisEquationUtils
         * @brief A static class providing all the necessary fuctions derived from the
         *     Friis Transmission Equation. See https://www.antenna-theory.com/basics/friis.php
         * TODO: insert a gitlab wiki link instead of provided above
         */
        class FriisEquationUtils
        {
            public:
            // Should be initialization performed in .cpp file?
            static const double c;  // Speed of light expressed in m/s
            
            /*!
             * @brief convert power expressed in Watts to dBW.
             * @param _power_abs power expressed in Watts.
             * @returns converted power expressed in dBW.
             * comment@throws std::invalid_argument if _power_abs <= 0.0, or is invalid number
             */
            static double watts_to_dBW(double _power_abs);

            /*!
             * @brief convert power expressed in Watts to dBm.
             * @param _power_abs power expressed in Watts.
             * @returns converted power expressed in dBm.
             * comment@throws std::invalid_argument if _power_abs <= 0.0, or is invalid number
             */
            static double watts_to_dBm(double _power_abs);

            /*!
             * @brief convert power expressed in dBW to Watts.
             * @param _power_dBW power expressed in dBW.
             * @returns converted power expressed in Watts.
             * comment@throws std::invalid_argument if _power_dBW is invalid number
             */
            static double dBW_to_watts(double _power_dBW);

            /*!
             * @brief convert power expressed in dBm to Watts.
             * @param _power_dBW power expressed in dBm.
             * @returns converted power expressed in Watts.
             * comment@throws std::invalid_argument if _power_dBm is invalid number
             */
            static double dBm_to_watts(double _power_dBm);

            /*!
             * @brief Calculate approximate received power, expressed in chosen units,
             * according to electromagnetic wave isotropic propagation damping. Both power and gain parameters could be expressed
             * as a ratio between two signal strengths - for non-decibels units, or as a dBi ratio for default decibel units.
             * The default gain argument value of "-1" means that no isotropic gain/damping is present in antennas (==1 for absolute values, or 0 for decibels).
             * FIXME: BUG: REVIEW:  the default arguments value specifying is impossible implicitly and incorrect!
             * @param _tx_power Transmitter output power, expressed in chosen units.
             * @param _frequency Frequency of the carrier wave, expressed in Hz (sic!).
             * @param _distance Distance between transmitter and receiver, expressed in meters.
             * @param _tx_gain FIXME: Assumed transmitter antenna gain, expressed in chosen units.
             * @param _rx_gain FIXME: Assumed receiver antenna gain, expressed in chosen units.
             * @param _ambient_gain FIXME: supposed gain/damping originating from other factors, e.g. ambient losses etc.
             * expressed in chosen units.
             * @param _decibels specify explicitly if the operated units are decibels, or absolute ones. 
             * Default true (means decibels are assumed).
             * @returns Calculated received power, expressed in chosen units.
             * comment@throws std::invalid_argument if some of the provided arguments are invalid.
             */
            static double estimate_rx_power(double _tx_power, double _frequency, double _distance, \
            double _tx_gain=-1.0, double _rx_gain=-1.0, double _ambient_gain=-1.0, bool _decibels=true);  // throw(std::invalid_argument);
            
            /*!
             * @brief Calculate approximate radiated power, expressed in chosen units,
             * according to electromagnetic wave isotropic propagation damping. Both power and gain parameters could be expressed
             * as a ratio between two signal strengths - for non-decibels units, or as a dBi ratio for default decibel units.
             * The default gain argument value of "-1" means that no isotropic gain/damping is present in antennas (==1 for absolute values, or 0 for decibels).
             * FIXME: BUG: REVIEW:  the default arguments value specifying is impossible implicitly and incorrect!
             * @param _rx_power Received power, expressed in chosen units.
             * @param _frequency Frequency of the carrier wave, expressed in Hz (sic!).
             * @param _distance Distance between transmitter and receiver, expressed in meters.
             * @param _tx_gain FIXME: Assumed transmitter antenna gain, expressed in chosen units.
             * @param _rx_gain FIXME: Assumed receiver antenna gain, expressed in chosen units.
             * @param _ambient_gain FIXME: supposed gain/damping originating from other factors, e.g. ambient losses etc.
             * expressed in chosen units.
             * @param _decibels specify explicitly if the operated units are decibels, or absolute ones. 
             * Default true (means decibels are assumed).
             * @returns Calculated transmitted power, expressed in chosen units. 
             * @note Nothrow version - returned value could be NaN.
             * comment@throws std::invalid_argument if some of the provided arguments are invalid.
             */
            static double estimate_tx_power(double _rx_power, double _frequency, double _distance, \
            double _tx_gain=-1.0, double _rx_gain=-1.0, double _ambient_gain=-1.0, bool _decibels = true);  // throw(std::invalid_argument);            

            /*!
             * @brief Calculate approximate distance between radiator and receiver, expressed in meters,
             * according to electromagnetic wave isotropic propagation damping. Both power and gain parameters could be expressed
             * as a ratio between two signal strengths - for non-decibels units, or as a dBi ratio for default decibel units.
             * The default gain argument value of "-1" means that no isotropic gain/damping is present in antennas (==1 for absolute values, or 0 for decibels).
             * FIXME: BUG: REVIEW:  the default arguments value specifying is impossible implicitly and incorrect!
             * @param _tx_power Transmitted output power, expressed in chosen units.
             * @param _rx_power Received power, expressed in chose units.
             * @param _frequency Carrier Wave frequency, expressed in Hz (sic!).
             * @param _tx_gain assumed transmitter antenna gain, expressed in chosen units.
             * @param _rx_gain assumed receiver antenna gain, expressed in chosen units.
             * @param _ambient_gain other sources of signal amplifying or damping, expressed in chosen units.
             * @param _decibels specify explicitly if the operated units are decibels, or absolute ones.
             * Default true (means that decibels are assumed).
             * @returns Calculated distance between transmitter and receiver, expressed in meters.
             * comment@throws std::invalid_argument if some of the provided arguments are invalid. 
             */
            static double estimate_distance(double _tx_power, double _rx_power, double _frequency, \
            double _tx_gain=-1.0, double _rx_gain=-1.0, double _ambient_gain=-1.0, bool _decibels = true);  // throw(std::invalid_argument);

            /*!
             * @brief Estimate minimal and maximal possible radiator signal strengths, for two given reads of the same signal.
             * @warning the non-decibel, absolute-units approach is not implemented yet.
             * @param _jtn_1 jammer to noise signal from first probe, expressed in chosen units.
             * @param _jtn_2 jammer to noise signal from second probe, expressed in chosen units.
             * @param _distance_between_probes Distance between probes expressed in meters.
             * @param _frequency Frequency of the carrier wave expressed in Hz (sic!).
             * @param _low_bound Pointer to the low power bound output variable.
             * @param _high_bound Pointer to the high power bound output variable.
             * @param _decibels Specify explicitly if the operated units are decibels, or absolute ones.
             * Default true (means that the decibels are assumed).
             * @returns nothing
             * @throws std::invalid_argument if some of the provided arguments are invalid.
             * comment@throws std::runtime_error if the signal strength bounds could not be calculated due to runtime error.
             */
            static void estimate_power_bounds(double _jtn_1, double _jtn_2, double _frequency, double _distance_between_probes, \
            double* _low_bound, double* _high_bound, bool _decibels=true); // throw(std::invalid_argument, std::runtime_error);
        
        };

        class ILocalizer
        {
            //virtual 
        };

        typedef struct cache_item
        {
            std::shared_ptr<Threat> pThreat; ///< Shared pointer to the threat
            std::map<uint32_t,std::shared_ptr<Signature>> signatures; ///< List of shared pointers to signatures associated with threat
        }cache_item_t; ///< Signle cache item that stores threat and list of associated to it signatures

        typedef std::unordered_map<uint64_t,cache_item_t> bucket_t; ///< Associative array for the cache items of the same threat type
        typedef std::vector<bucket_t> cache_map_t; ///< Vector containing all possible buckets. Cache map

    private:

        ITimeUtils * time_utils; ///< Time related utilites
        cache_map_t active_cache; ///< Cache for active threats and signatures
        std::unordered_map<uint32_t,GeolocationData> probe_geolocation_cache; ///< Cahce for holding probes positions
        static uint64_t unique_threat_id_counter; ///< unique threat id counter used to generate new threat ids
        std::deque<std::pair<uint32_t,encoderThreat>> signatures_queue; ///< Incoming queue of signatures recieved from different probes. From this queue signatures are picked up by the ActiveThreatsManager thread in order to classify them.
        std::mutex signatures_queue_mutex; ///< signatures_queue mutex for push operation
        std::condition_variable condition_var; ///< This variable will notifiy if signatures_queue has changed in a blocking way ( to suspend ActiveThreatsManager thread if no signatures were pushed into incoming queue )
        std::thread worker_thread; ///< Thread handler for signature classifications
        bool is_worker_running; ///< Status flag. Set to true if signature classification thread is running
        ThreatClassifier threat_classifier; ///< Classifier of the threats
        /*!
         * @brief Deletes expired active threats and signatures from cache and push them into database
         */
        class Watchdog {

            friend class ActiveThreatsManagerTest;

                cache_map_t * cache; ///< Pointer to monitored cache
                uint32_t threat_life_period_ms; ///< The maximum active threat lifetime if the threat was not updated
                std::thread watchdog_thread; ///< Watchdog thread
                bool is_running; ///< Status flag. Set to true if watchdog thread is running
                ITimeUtils * time_utils;
                /*!
                 * @brief Returns list of the active threats that have expired and should be moved into database.
                 * @param bucket a cache bucket to check for expired threats
                 * @param threat_life_period_ms the maximum active threat lifetime if the threat was not updated
                 * @return list of expired threats unique identifiers
                 */
                static std::list<uint64_t> getExpiredThreatsFromBucket(const ActiveThreatsManager::bucket_t & bucket, uint32_t threat_life_period_ms);
                /*!
                 * @brief Removes specified threats from the cache bucket
                 * @param bucket a cache bucket where threats to delete are stored
                 * @param threats_to_delete list of unique identifiers of threats to delete
                 */
                static void deleteThreatsFromBucket(bucket_t &bucket, std::list<uint64_t> & threats_to_delete);

                /*!
                 * @brief A main function for Watchdog thread
                 */
                void worker();
            public:
                /*!
                 * @brief Parameterized constructor
                 * @param cache a reference to the cache to monitor
                 * @param threat_life_period life time of the active threat in millisecods.
                 * If active threat is not be updated during this period it will be moved from the cache to the database
                 */
                Watchdog(cache_map_t * cache, ITimeUtils * time_utils, uint32_t threat_life_period=15000);
                ~Watchdog();
                /*!
                 * @brief Stops watchdog and waits for its thread to finish
                 */
                void stop();
                /*!
                 * @brief Searches all cache for expired threats and deletes them and their signatures
                 * @param cache a pointer to the cache to check
                 * @param threat_life_period_ms the maximum active threat lifetime if the threat was not updated
                 */
                static void checkForExpiredThreats(cache_map_t * cache, uint32_t threat_life_period_ms);
        };
        /*!
         * @brief Creates new unique threat identifier
         * @return new unique threat identifier
         */
        static uint64_t createNewThreatId();

        /*!
         * @brief Estimates radiator power, based on data from unique probes.
         * At least 6 active probes are required.
         * @param pThreat reference to Threat object
         * @param threat_descriptor a new signature
         * @return nothing
         * @throws std::runtime_error
         */
        void updateEIRP(Threat &pThreat, const encoderThreat &threat_descriptor) throw(std::runtime_error);
        /*!
         * @brief Updates threat geolocation data according to given new threat.
         * Using Eigen Dense module. Using MatrixXD, VectorXD and Vector2D types and jacobiSVD calculation method.
         * @param Threat, pointer to threat object for which we're updating geolocation data
         * @param probe_unique_id, this parameter is deprecated.
         * @param threat_descriptor, a new signature came from the given probe
         */
        void updateGeolocation(Threat &pThreat, uint32_t probe_unique_id, const encoderThreat &threat_description) throw(std::range_error);
        //void updateGeolocation(GeolocationData * threat_position, const GeolocationData & probe_position, const ActiveThreatsManager::encoderThreat & threat_descriptor);
        /*!
         * @brief Updates threat geolocation based on received signatures.
         * @param pThreat, reference to threat object for which we're updating geolocation data
         * @param threat_descriptor, a new signature came from the given probe
        */
        void updateGeolocation2(Threat& pTthreat, const encoderThreat &threat_description);
        /*!
         * @brief Pops signature threat from signatures queue. This function is blocking
         * @return std::pair containing probe unqique identifier and signature
         */
        std::pair<uint32_t,encoderThreat> pop();
        /*!
         * @brief Main function for signatures classification thread
         */
        void worker();
    public:
        Watchdog * watchdog; ///< Watchdog for active threats cache
        /*!
         * @brief retrives an active threat from active threats cache based on threat unique id
         * This function extracts active threat from cache. This operation can be done with threat unqiue id only,
         * however, threat type is also given to ensure faster lookup into active cache map
         * @param threat_type, a threat type for which active threat should be returned
         * @param threat_unique_id, threat unique id for which active threat should be returned
         * @return shared pointer to the requested active threat if found. Null pointer if threat does not exist in active threats cache
         */
        std::shared_ptr<Threat> getActiveThreat(ThreatClassifier::threat_type_t threat_type, uint64_t threat_unique_id);
        /*!
         * @brief Get all active threats based on threat type
         * @param threat_type, a threat type for which active threats should be returned
         * @param probe_unique_id, a probe unique identifier given if returned active threats need to be within coverage region of that probe
         * @return list of shared pointers to active threats
         */
        std::list<std::shared_ptr<Threat> > getActiveThreats(ThreatClassifier::threat_type_t threat_type, uint32_t probe_unique_id=0);
        /*!
         * @brief Returns probe position based on it's unique identifier
         * @param probe_unique_id, a probe unique identifier for which position should be returned
         * @return reference to the structure containing probe geolocation data If probe id is not vaild, geolocation data with NAN values is returned.
         */
        const GeolocationData & getProbePosition(uint32_t probe_unique_id);
        /*!
         * @brief Appends new signature recieved from probe to the active threats manager
         * @param probe_unique_id, a probe identifier from which signature was received
         * @param threat_unique_id, threat unique id retrived from ThreatClassifier. If 0: new threat will be created for specified threat descriptor
         * @param threat_descriptor, a classified threat descriptor for which threat unique id was matched by ThreatClassifier
         * @return pointer to the cache_item_t object where signature was appended into active chace
         */
        cache_item_t * appendSignature(uint32_t probe_unique_id, uint64_t threat_unique_id, const encoderThreat & threat_descriptor);
        /*!
         * @brief Gets specified by threat_type bucket from the active cache. Bucket is associative array of all threats with the same threat type.
         * Note, threat_type is not asserted inside this method for efficiency.
         * @param threat_type, a threat type for which active threats should be returned
         * @return pointer to the bucket_t
         */
        bucket_t * getBucket(ThreatClassifier::threat_type_t threat_type);
        /*!
         * @brief Pushes newly recieved signature to the collective queue. All signatures from this queue will be automatically picked and classified by
         * ActiveThreatsManager own thread. This function is thread safe
         * @param probe_unique_id, a probe identifier from which signature was received
         * @param signature recieved by the probe threat signature
         * @param latitude of probe that signature came from
         * @param longitude of probe that signature came from
         */
        void pushSignature(uint32_t probe_unique_id, const encoderThreat & signature, double latitude, double longitude);
        /*!
         * @brief Parameterized constructor
         * @brief active_threats_lifetime_ms specifies maximum time period in milliseconds that not updated threats will live in active cache.
         * After this time expired threats and their signatures will be moved to the database and deleted from cache. Default value is 5000 ( 5 seconds ).
         * If this value is set to 0, threats will live in cache infinitely
         */
        ActiveThreatsManager(uint32_t active_threats_lifetime_ms=5000);
        ~ActiveThreatsManager();
        /*!
         * @brief Starts internal signatures classification thread. Newly incoming signatures can be processed by this thread by pushing them into incoming queue.
         * This can be done by use of ActiveThreatsManager::pushSignature method
         */
        void start();
};

#endif // ACTIVETHREATSMANAGER_H
