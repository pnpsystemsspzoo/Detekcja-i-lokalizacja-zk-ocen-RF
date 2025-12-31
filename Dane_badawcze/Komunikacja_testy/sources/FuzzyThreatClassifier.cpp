#include "FuzzyThreatClassifier.h"
#include <ios>

FuzzyThreatClassifier::FuzzyThreatClassifier(const char *fcl_file_path)
{
    // create and load the model
    this->ffll_model = ffll_new_model();
    int ret_val = (int)ffll_load_fcl_file(this->ffll_model, fcl_file_path);
    if (ret_val < 0)
    {
        std::string err_msg;
        err_msg = "Error opening fcl file: ";
        err_msg += fcl_file_path;
        err_msg += "\nMake sure file exists in the specified path";
        throw std::ios_base::failure(err_msg);
    }
    // create a child for the model...
    this->ffll_child = ffll_new_child(this->ffll_model);
}

FuzzyThreatClassifier::signal_t FuzzyThreatClassifier::feed(const input_t &input_data)
{
    ffll_set_value(this->ffll_model, this->ffll_child, SWEEP, input_data.sweep);
    ffll_set_value(this->ffll_model, this->ffll_child, F_B, input_data.f_B);
    ffll_set_value(this->ffll_model, this->ffll_child, SYMMETRY_RATIO, input_data.symetry_ratio);
    ffll_set_value(this->ffll_model, this->ffll_child, POWER_RATIO, input_data.power_ratio);
    return static_cast<signal_t>(ffll_get_output_value(this->ffll_model, this->ffll_child));
}
