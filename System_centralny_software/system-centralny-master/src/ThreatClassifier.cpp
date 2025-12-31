#include "ThreatClassifier.h"
#include "ActiveThreatsManager.h"

/* This is an small extension in std since gcc lower than 5 has set of libraries
 * that cannot instantiate unordered_set template class with enums
 */
#if __GNUC__ < 5
    namespace std
    {
        template<typename E>
        struct hash
        {
           typedef E argument_type;
           typedef size_t result_type;
           using sfinae = typename std::enable_if<std::is_enum<E>::value>::type;

           result_type operator() (const E& e) const
           {
              using base_t = typename std::underlying_type<E>::type;
              return std::hash<base_t>()(static_cast<base_t>(e));
           }
        };
    }
#endif


typedef ThreatClassifier::ThreatModel::feature_vector_t feature_vector_t;
typedef ThreatClassifier::ThreatModel::threat_feature feature;
typedef ThreatClassifier::ThreatModel::parameter parameter;
typedef ThreatClassifier::threat_type_t THREAT_TYPE;

// TODO set proper feature templates for ThreatClassifier
ThreatClassifier::ThreatModel::features_map_t ThreatClassifier::ThreatModel::features_templates_map {
    {THREAT_TYPE::UNKNOWN,
        {
            feature(parameter::sygn_BW,0.9),
            feature(parameter::sygn_f0,0.9),
            feature(parameter::sygn_J,0.9),
            feature(parameter::sygn_No,0.9)
        }
    },
    {THREAT_TYPE::WIDE_SWEEP_SLOW,
        {
            feature(parameter::sygn_f0,0.9)
        },
    },
    {THREAT_TYPE::WIDE_SWEEP_MEDIUM,
        {
            feature(parameter::sygn_f0,0.9)
        },
    },
    {THREAT_TYPE::WIDE_SWEEP_FAST,
        {
            feature(parameter::sygn_f0,0.9)
        },
    },
    {THREAT_TYPE::WIDE_SWEEP_RAPID,
        {
            feature(parameter::sygn_f0,0.9)
        },
    },
    {THREAT_TYPE::NARROW_SWEEP,
        {
            feature(parameter::sygn_f0,0.9)
        },
    },
    {THREAT_TYPE::TRIANGULAR_WAVE,
        {
            feature(parameter::sygn_f0,0.9)
        },
    },
    {THREAT_TYPE::TRIANGULAR,
        {
            feature(parameter::sygn_f0,0.9)
        },
    },
    {THREAT_TYPE::SAWTOOTH,
        {
            feature(parameter::sygn_f0,0.9)
        },
    },
    {THREAT_TYPE::HOOKED_SAWTOOTH,
        {
            feature(parameter::sygn_f0,0.9)
        },
    },
    {THREAT_TYPE::TICK,
        {
            feature(parameter::sygn_f0,0.9)
        },
    },
    {THREAT_TYPE::MULTI_TONE,
        {
            feature(parameter::sygn_f0,0.9)
        },
    }
};

#define PARAMETER_VALUE 0
#define PARAMETER_VARIANCE 1
#define MAP_PARAMETER_CONST_VARIANCE(_X, _VARIANCE)       {ThreatClassifier::ThreatModel::_X,{[](const ThreatClassifier::threat_descriptor_t & threat_descriptor){return static_cast<double>(threat_descriptor._X);},                [](const ThreatClassifier::threat_descriptor_t & threat_descriptor){return static_cast<double>(_VARIANCE);}}}
#define MAP_CHIRP_PARAMETER_CONST_VARIANCE(_X, _VARIANCE) {ThreatClassifier::ThreatModel::_X,{[](const ThreatClassifier::threat_descriptor_t & threat_descriptor){return static_cast<double>(threat_descriptor.sygn_data.chirp._X);},[](const ThreatClassifier::threat_descriptor_t & threat_descriptor){return static_cast<double>(_VARIANCE);}}}
#define MAP_PARAMETER_VARIABLE_VARIANCE(_X, _VARIANCE)    {ThreatClassifier::ThreatModel::_X,{[](const ThreatClassifier::threat_descriptor_t & threat_descriptor){return static_cast<double>(threat_descriptor._X);},                [](const ThreatClassifier::threat_descriptor_t & threat_descriptor){return static_cast<double>(threat_descriptor._VARIANCE);}}}

ThreatClassifier::ThreatModel::threat_feature::parameters_map_t ThreatClassifier::ThreatModel::threat_feature::parameters_vectors_map {
            MAP_PARAMETER_VARIABLE_VARIANCE(sygn_f0,sygn_f0_var),
            MAP_PARAMETER_VARIABLE_VARIANCE(sygn_BW,sygn_BW_var),
            MAP_PARAMETER_CONST_VARIANCE(sygn_No,SYGN_NO_VARIANCE),
            MAP_PARAMETER_CONST_VARIANCE(sygn_J,SYGN_J_VARIANCE),
            MAP_CHIRP_PARAMETER_CONST_VARIANCE(sygn_chirp_sweep,SYGN_CHIRP_SWEEP_VARIANCE),
            MAP_CHIRP_PARAMETER_CONST_VARIANCE(sygn_chirp_period,SYGN_CHIRP_PREDIOD_VARIANCE),
            MAP_CHIRP_PARAMETER_CONST_VARIANCE(sygn_chirp_return,SYGN_CHIRP_RETURN_VARIANCE)
};

// inverse error function approximation up to 1E-6
double erfInvf(double x)
{
   double tt1, tt2, lnx, sgn;
   sgn = (x < 0) ? -1.0f : 1.0f;

   x = (1 - x)*(1 + x);        // x = 1 - x*x;
   lnx = log(x);

   tt1 = 2/(M_PI*0.147) + 0.5f * lnx;
   tt2 = 1/(0.147) * lnx;

   return(sgn*sqrt(-tt1 + sqrt(tt1*tt1 - tt2)));
}
double ThreatClassifier::ThreatModel::threat_feature::normalCDF(double value, double mu, double sigma)
{
    double res = 0.5*(1+erf((value -mu)/sigma*M_SQRT1_2));
    return res;
}

double ThreatClassifier::ThreatModel::threat_feature::getProbabilityIsInRange(double lower_bound, double upper_bound, double mu, double sigma)
{
    if(upper_bound <= lower_bound) return 0;

    double P = normalCDF(upper_bound,mu,sigma) - normalCDF(lower_bound,mu,sigma);
    return P;
}

// returns probit for specified probability i.e. inverse normal CDF
double probit(double P)
{
    return M_SQRT2*erfInvf(2*P -1);
}


const ThreatClassifier::ThreatModel::feature_vector_t &ThreatClassifier::ThreatModel::getFeaturesTemplate(threat_type_t type)
{
    return features_templates_map.at(type);
}

ThreatClassifier::ThreatModel::ThreatModel(uint64_t threat_unique_id, const ThreatClassifier::threat_descriptor_t &threat) :
    features(getFeaturesTemplate(threat.getType())),
    unique_id(threat_unique_id),
    type(threat.getType())
{
    for(auto & f : features)
    {
        f.init(threat);
    }
}

double ThreatClassifier::ThreatModel::getProbability(const ThreatClassifier::threat_descriptor_t &threat)
{
    if(features.size() == 0) return 0;
    double nom=0;
    double denom=0;
    for(auto & f: features)
    {
        double weight = threat_feature::getProbabilityIsInRange(-f.relative_confidence_interval, f.relative_confidence_interval);
        nom += f.getProbability(threat)*weight;
        denom += weight;
    }
    return nom/denom;
}

bool ThreatClassifier::ThreatModel::classifyThreat(const ThreatClassifier::threat_descriptor_t &threat)
{
    /*for(auto & f : features)
    {
        if(!f.isMatching(threat))
        {
            return false;
        }
    }*/
    size_t ct = features.size();
    for(int i = 0; i < ct; i++)
    {
        if(!features[i].isMatching(threat))
        {
            return false;
        }
    }
    return true;
}

void ThreatClassifier::ThreatModel::update(const ThreatClassifier::threat_descriptor_t &threat)
{
    for(auto & f: features)
    {
        f.update(threat);
    }
}

double ThreatClassifier::ThreatModel::threat_feature::extractParameterValue(const ThreatClassifier::threat_descriptor_t &threat)
{
    return ThreatModel::threat_feature::parameters_vectors_map.at(this->k)[PARAMETER_VALUE](threat);
}

double ThreatClassifier::ThreatModel::threat_feature::extractParameterVariance(const ThreatClassifier::threat_descriptor_t &threat)
{
    return ThreatModel::threat_feature::parameters_vectors_map.at(this->k)[PARAMETER_VARIANCE](threat);
}

double ThreatClassifier::ThreatModel::threat_feature::getProbability(const ThreatClassifier::threat_descriptor_t &threat)
{
    double x =  this->extractParameterValue(threat);
    double x_sigma = sqrt(this->extractParameterVariance(threat));
    double range = relative_confidence_interval*this->sigma;
    double upper_bound = this->mu+range;
    double lower_bound = this->mu-range;
    double res = getProbabilityIsInRange(lower_bound,upper_bound,x,x_sigma);
    return res;
}

bool ThreatClassifier::ThreatModel::threat_feature::isMatching(const ThreatClassifier::threat_descriptor_t &threat)
{
    double x = extractParameterValue(threat);
    double confidence_interval = this->sigma*this->relative_confidence_interval;
    return (abs(x-this->mu) <= confidence_interval);
}

ThreatClassifier::ThreatModel::threat_feature::threat_feature(ThreatClassifier::ThreatModel::threat_parameter_t parameter_enumerator, double relative_confidence_interval):
    k(parameter_enumerator),
    relative_confidence_interval(relative_confidence_interval)
{

}

void ThreatClassifier::ThreatModel::threat_feature::init(const ThreatClassifier::threat_descriptor_t &threat)
{
    mu = extractParameterValue(threat);
    variance = extractParameterVariance(threat);
    sigma = sqrt(variance);
}

void ThreatClassifier::ThreatModel::threat_feature::update(const ThreatClassifier::threat_descriptor_t &threat)
{
    double x = extractParameterValue(threat);
    double v_x = extractParameterVariance(threat);
    double denom_inv = 1/(variance + v_x);

    // according to the convolution of two gaussian pdfs
    mu    = (mu*v_x + x*variance)*denom_inv;
    sigma = (sigma*v_x + sqrt(v_x)*variance)*denom_inv;
    variance = variance*v_x*denom_inv;
}

uint64_t ThreatClassifier::classify(const ThreatClassifier::threat_descriptor_t &signature_threat, uint32_t probe_unique_id)
{
    // peek active threats
    auto threats = active_threats_manager->getActiveThreats(signature_threat.getType(),probe_unique_id);
    // loop through all threats models
    int n = threats.size();
    std::list<std::shared_ptr<ActiveThreatsManager::Threat>> classified;
    for(auto & pThreat : threats)
    {
        if(pThreat->model.classifyThreat(signature_threat))
        {
            classified.push_back(pThreat);
        }
    }
    int n_c = classified.size();
    if(n_c > 0)
    {
        if(n_c > 1)
        {
            // multiple threats classified
            // choose one with the biggest probability
            auto max_item = classified.front().get();
            double max_P = max_item->model.getProbability(signature_threat);
            for(auto mIter = std::next(classified.begin()); mIter != classified.end(); ++mIter)
            {
                double P = mIter->get()->model.getProbability(signature_threat);
                if(P > max_P)
                {
                    max_P = P;
                    max_item = mIter->get();
                }
            }
            return max_item->getUniqueId();
        }else
        {
            // only one threat classified
            return classified.front()->getUniqueId();
        }

    }else
    {
        return 0UL;
    }


}

ThreatClassifier::ThreatClassifier(ActiveThreatsManager *threats_manager) :
    active_threats_manager(threats_manager)
{

}
