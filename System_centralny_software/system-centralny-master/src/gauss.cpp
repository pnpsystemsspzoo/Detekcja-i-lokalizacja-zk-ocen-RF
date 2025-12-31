#include "gauss.h"


GaussianDistribution::GaussianDistribution(double _mean, double _variance, double _weight)
{
    if(std::isinf(_mean) || std::isinf(_variance) || std::isinf(_weight))
    {
        throw invalid_argument("Some of provided arguments are not finite!");
    }
    else if(std::isnan(_mean) || std::isnan(_variance) || std::isnan(_weight))
    {
        throw invalid_argument("Some of provided argumments are NaN!");
    }
    else if(_weight < 0.0)
    {
        throw invalid_argument("Weight argument cannot be negative!");
    }
    else if(_variance < 0.0)
    {
        throw invalid_argument("Variance argument cannot be negative!");
    }
    // Checks passed - continue initialization
    this->mean = _mean;
    this->variance = _variance;
    this->weight = _weight;
}

double GaussianDistribution::get_mean()
{
    return this->mean;
}

double GaussianDistribution::get_variance()
{
    return this->variance;
}

double GaussianDistribution::get_weight()
{
    return this->weight;
}

void GaussianDistribution::set_mean(double _new_mean)
{
    if(std::isinf(_new_mean) || std::isnan(_new_mean))
    {
        throw invalid_argument("Invalid mean provided!");
    }
    this->mean = _new_mean;
}

void GaussianDistribution::set_variance(double _new_variance)
{
    if(std::isinf(_new_variance) || std::isnan(_new_variance))
    {
        throw invalid_argument("Invalid variance provided!");
    }
    this->variance = _new_variance;
}

void GaussianDistribution::set_weight(double _new_weight)
{
    if(_new_weight == 0)
    {
        throw std::invalid_argument("Invalid weight provided!");
    }
    this->weight = _new_weight;
}

GaussianDistribution GaussianDistribution::operator +(const GaussianDistribution& that)  // throw(std::range_error)
{
    // if(this->weight + that->weight > UINT32_MAX)
    // {
    //     throw range_error("sum of ");
    // }
    GaussianDistribution temp;
    temp.weight = this->weight + that.weight;

    temp.mean = (this->mean*this->weight + that.mean*that.weight) / temp.weight;
    
    // for variance equation, see https://stats.stackexchange.com/questions/205126/
    temp.variance = (this->variance*this->weight + that.variance*that.weight + (that.mean - this->mean)\
    *(that.mean - this->mean)*this->weight*that.weight/temp.weight)/temp.weight;

    return temp;
}

GaussianDistribution& GaussianDistribution::operator +=(const GaussianDistribution& that)  // throw(st::range_error)
{
    // if(this->weight + that.weight > UINT32_MAX)
    // {
    //     throw range_error("");
    // }
    double temp_weight = this->weight + that.weight;

    double temp_mean = (this->mean*this->weight + that.mean*that.weight) / temp_weight;
    
    // for variance equation, see https://stats.stackexchange.com/questions/205126/
    double temp_variance = (this->variance*this->weight + that.variance*that.weight + (that.mean - this->mean)\
    *(that.mean - this->mean)*this->weight*that.weight/temp_weight)/temp_weight;

    this->mean = temp_mean;
    this->variance = temp_variance;
    this->weight = temp_weight;

    return *this; // XXX
}
