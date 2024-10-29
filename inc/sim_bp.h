#ifndef SIM_BP_H
#define SIM_BP_H

#include <cstdint>
#include <iostream>

typedef struct bp_params{
    unsigned long int K;
    unsigned long int M1;
    unsigned long int M2;
    unsigned long int N;
    char*             bp_name;
}bp_params;

// Put additional data structures here as per your requirement

class BranchPredictor {
    private:

    uint32_t m;
    uint32_t n;
    uint32_t PT_len;  // Prediction Table length

    public:

    uint32_t num_predictions;
    uint32_t num_mispredictions;

    uint8_t *prediction_table;
    uint32_t gbhr;  // Global Branch History Register
    std::string predictor;

    BranchPredictor() = default;
    ~BranchPredictor() = default;

    uint32_t parseBranchPC(uint32_t);

    void BP_Init(uint32_t, uint32_t, std::string);
    void Print_Contents(void);

    uint8_t Prediction_Value(uint32_t, uint32_t &);
    void Impl_Bimodal(uint32_t, bool);
    void Impl_Gshare(uint32_t, bool);
};

#endif
