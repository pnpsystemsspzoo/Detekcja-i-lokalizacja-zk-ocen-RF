#ifndef FUZZYTHREATCLASSIFIER_H
#define FUZZYTHREATCLASSIFIER_H

#include "probeMessagesEncoder.h"
#include "ffll/FFLL.h"	// FFLL API

// List of input variables (appearance order )defined in *.fcl file
#define SWEEP           0 // sweep rate
#define F_B             1 // freq. bandwidth
#define SYMMETRY_RATIO  2 // (sweep - return)
#define POWER_RATIO     3 // sygn_J/sygn_No


class FuzzyThreatClassifier
{
    private:

        int ffll_model;
        int ffll_child;

    public:

        typedef ProbeMessagesEncoder::messageThreat::signal_type_t signal_t;

        typedef struct input
        {
            double sweep;
            double f_B;
            double symetry_ratio;
            double power_ratio;
        }input_t;

        FuzzyThreatClassifier(const char* fcl_file_path);
        signal_t feed(const input_t & input_data);
};

#endif // FUZZYTHREATCLASSIFIER_H
