#ifndef THREATCLASSIFIER_H
#define THREATCLASSIFIER_H

#include <vector>
#include "probeMessagesEncoder.h"
#include <stdint.h>
#include <unordered_map>
#include <memory>
#include <math.h>

// TODO: Set proper variances constants ( Not neccesserry for proper operation )
#define SYGN_NO_VARIANCE 100.0
#define SYGN_J_VARIANCE 200.0
#define SYGN_CHIRP_SWEEP_VARIANCE 300.0
#define SYGN_CHIRP_PREDIOD_VARIANCE 400.0
#define SYGN_CHIRP_RETURN_VARIANCE 500.0

class ActiveThreatsManager;
/*!
 * @brief Naive Bayesian based probe signature classifier
 * This class implements modified Naive Bayesian Classifier algorithm to classify probe signature to appropriate threat
 */
class ThreatClassifier
{
    friend class ThreatClassifierTest;

    public:

        typedef ProbeMessagesEncoder::messageThreat::threat_descriptor_t threat_descriptor_t; ///< Alias to signature threat descriptor
        typedef ProbeMessagesEncoder::messageThreat::signal_type_t threat_type_t; ///< Alias to signature signal_type_t
        /*!
         * @brief Representation class for threat model
         * This class represents statistical threat model that estimates threat's features
         */
        class ThreatModel
        {
            friend class ThreatClassifierTest;

            public:
                typedef enum parameter
                {
                    sygn_f0 = 0, ///< central frequency
                    sygn_BW = 1, ///< bandwidth
                    sygn_No = 2, ///< noise power
                    sygn_J = 3, ///< signal power
                    sygn_chirp_sweep = 4, ///< signal sweep rate
                    sygn_chirp_period = 5, ///< signal period
                    sygn_chirp_return = 6 ///< signal return time
                }threat_parameter_t; ///< Threat model parameters enumerator. These are keys that map to signature threat parameters

                typedef struct threat_feature
                {
                    friend class ThreatModel;
                    friend class ThreatClassifierTest;

                    private:
                        threat_parameter_t k; ///< key mapping to the signature parameter
                        double relative_confidence_interval; ///< confidence interval related to standard deviation i.e. value 1 corresponds to confidence interval with one sigma error (aprox. 68%)

                        typedef std::unordered_map<threat_parameter_t,std::vector<double(*)(const ThreatClassifier::threat_descriptor_t &)>> parameters_map_t; ///< threat feature key to signature parameter map type definition
                        static parameters_map_t parameters_vectors_map; ///< Feature key to signature parameter map
                        /*!
                         * @brief Calculates value of the Cumulative Distribution Function for normal distribution.
                         * @param x a CDT function argument
                         * @param mu expectation value of the distribution. Default is zero
                         * @param sigma standard deviation of the distribution. Default is 1
                         * @return Probability
                         */
                        static double normalCDF(double x, double mu=0, double sigma=1);
                        /*!
                         * @brief Return probability that expectation value is within interval
                         * @param lower_bound a lower bound of the interval
                         * @param upper_bound a uppar bound of the interval
                         * @param mu expectation value for distribution to check
                         * @param sigma standard deviation for distribution to check
                         * @return Probability that the expectation value is within interval [ lower_bound; upper_bound ]
                         */
                        static double getProbabilityIsInRange(double lower_bound, double upper_bound, double mu=0, double sigma=1);
                    public:
                        double mu; ///< estimated expected value
                        double sigma; ///< estimated standard deviation
                        double variance; ///< estimation error
                        /*!
                         * @brief extracts parameter value from threat descriptor based on the feature key 'k'
                         * This method calls converter function that preserves proper cast of the threat descriptor parameter
                         * @param threat, threat descriptor from which parameter should be extracted
                         * @return casted signature parameter value
                         */
                        double extractParameterValue(const threat_descriptor_t & threat);
                        /*!
                         * @brief extracts variance value from threat descriptor based on the feature key 'k'
                         * This method calls converter function that preserves proper cast of the threat descriptor variance
                         * @param threat, a threat descriptor from which variance should be extracted
                         * @return casted signature variance value. If appropriate variance does not exist in threat descriptor, predefined constant variance is returned
                         */
                        double extractParameterVariance(const threat_descriptor_t & threat);
                        /*!
                         * @brief Calculates probability that this feature matches given threat descriptor
                         * @param threat, a threat descriptor for which the existence of this feature needs to be checked
                         * @return Probability that this feature matches the given threat descriptor
                         */
                        double getProbability(const threat_descriptor_t & threat);
                        /*!
                         * @brief checks if feature matches to the specified threat i.e. if threat contains this feature.
                         * The Matching algorithm checks if P(f|S) >= 1-error (probability of the feature f existence, if signature S is given, is no less than 1 - predefined error margin)
                         * Predefined error margin can be set in constructor using relative_confidance_interval (See constructor)
                         * @param threat, a threat descriptor for which the existence of this feature need to be obtained
                         * @return True if probabilty that featue exist in given threat is grater than margin predefined by confidence interval
                         */
                        bool isMatching(const threat_descriptor_t & threat);
                        /*!
                         * @brief Constructor
                         * @param parameter_enumerator, a key value that map to the signature threat descriptor parameter
                         * @param relative_confidence_interval, a confidence interval related to the standard deviation i.e. value 1 corresponds to confidence interval with one sigma error (aprox. 68%)
                         */
                        threat_feature(threat_parameter_t parameter_enumerator, double relative_confidence_interval);
                        /*!
                         * @brief Sets initial values for expected value, standard deviation and modeling error based on the given threat descriptor
                         * @param threat, a threat descriptor for which feature initial values should be taken
                         */
                        void init(const threat_descriptor_t & threat);
                        /*!
                         * @brief Updates model predictions based on the new knowledge from the given threat descriptor
                         * @param threat, a threat descriptor from which data should be obtained to update feature model
                         */
                        void update(const threat_descriptor_t & threat);
                }threat_feature_t; ///< threat feature model type definition
                typedef std::vector<threat_feature_t> feature_vector_t; ///< alias to vector of the threat features type
                feature_vector_t features; ///< vector of the modeled threat features

                threat_type_t type; ///< threat type
                uint64_t unique_id; ///< unique id of the modeled threat

                typedef std::unordered_map<threat_type_t,feature_vector_t> features_map_t; ///< type definition of the map to the feature vectors templates
                static features_map_t features_templates_map; ///< features template map for different threat types
                /*!
                 * @brief Gets reference to the template feature vector for the given threat type
                 * @param threat, threat descriptor for which feature vector template should be returned
                 * @return vector of the feature templates for a given threat type.
                 */
                static const feature_vector_t & getFeaturesTemplate(ThreatClassifier::threat_type_t type);
                /*!
                 * @brief Constructor
                 * @param threat_unique_id, a threat unique identifier in database
                 * @param threat, threat descriptor for which threat model should be created
                 */
                ThreatModel(uint64_t threat_unique_id, const threat_descriptor_t & threat);
                /*!
                 * @brief Calculates probability that this threat model matches given threat descriptor
                 * To prevent discarding long feature vectors when classifing threat, this function calculates probability weighted average rather than multiplying each feature probability
                 * @param threat, a threat descriptor for which probability should be calculated
                 * @return Probability (0:1) that threat model matches given threat descriptor
                 */
                double getProbability(const threat_descriptor_t & threat);
                /*!
                 * @brief Checks if given threat descriptor matches threat model
                 * @param threat, a threat descriptor to classify
                 * @return True if threat descriptor matches threat model. Otherwise false
                 */
                bool classifyThreat(const threat_descriptor_t & threat);
                /*!
                 * @brief Updates threat model predictions based on the new knowledge from the given threat descriptor
                 * @param threat, a threat descriptor from which data should be obtained to update feature model
                 */
                void update(const threat_descriptor_t & threat);
        };
        /*!
         * @brief Classifies given signature
         * @param signature_threat, a threat descriptor to classify
         * @param probe_unique_id, a probe unique id from which threat decriptor was received
         * @return Unique id of the threat model that matches the given threat descriptor. If there is no matching threat model, 0 is returned.
         */
        uint64_t classify(const threat_descriptor_t & signature_threat, uint32_t probe_unique_id);
        ActiveThreatsManager * active_threats_manager; ///< pointer to the active threat manager
        /*!
         * @brief Constructor
         * @param threats_manager, pointer to the active threat manager
         */
        ThreatClassifier(ActiveThreatsManager * threats_manager=nullptr);
};

#endif // THREATCLASSIFIER_H
