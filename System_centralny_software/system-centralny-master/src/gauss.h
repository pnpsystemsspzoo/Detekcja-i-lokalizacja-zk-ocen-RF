#pragma once

#include <cstdint>
#include <stdexcept>
#include <cmath>

using namespace std;

/*!
 * @brief weighted Gaussian distribution structure.
 * Allows for mathematical operations on several instances 
 * (weighted sum of two gaussian probabilities).
 */
struct GaussianDistribution
{
public:

    /*!
     * @brief default constructor
     * @param _mean mean value, or expectation of the distribution.
     * @param _variance square of standard deviation, or variance of gaussian distribution.
     * @param _weight starting weight of the gaussian distribution instance. Is used for weighted sum calculations.
     * @throws std::invalid_argument
     */
    explicit GaussianDistribution(double _mean=0.0, double _variance=0.0, double _weight=1.0);

    //! @brief Mean attribute getter.
    double get_mean();
    //! @brief variance attribute getter.
    double get_variance();
    //! @brief weight attribute getter.
    double get_weight();

    //! @brief Mean attribute argument-safe setter.
    //! @throws std::invalid_argument if the mean value is Nan or Inf
    void set_mean(double _new_mean);

    //! @brief Variance attribute argument-safe setter.
    //! @throws std::invalid_argument if the variance is NaN or Inf
    void set_variance(double _new_variance);

    //! @brief Weight attribute argument-safe setter.
    //! @throws std::invalid_argument if the weight is NaN, Inf or less than 0
    void set_weight(double _new_weight);

    //! @brief Overloaded sum operator
    //! Performs weighted addition of two GaussianDistribution objects. Stores the result in newly created object.
    //! @returns weighted sum
    GaussianDistribution operator +(const GaussianDistribution& that);  // throw(std::range_error);

    //! @brief Overloaded sum-assignment operator
    //! Performs weighted addition of two GaussianDistribution objects. Stores the result in left operand, overwriting its value.
    //! @returns reference to the left operand
    GaussianDistribution& operator +=(const GaussianDistribution& that);  // throw(st::range_error);

private:
    
    double mean;  // mean value private attribute
    double variance;  // variance private attribute
    double weight;  // weight private attribute

};

typedef GaussianDistribution gauss_t;