#include<iostream>
#include <cstring>
#include <iomanip>
#include <cmath>

#include "sim_bp.h"

using namespace std;

void BranchPredictor :: BP_Init(uint32_t m, uint32_t n, std::string bp_name) {

  predictor = bp_name;

  this->m = m;
  this->n = n;
  this->PT_len = (unsigned)pow(2,m);

  prediction_table = new uint8_t[PT_len];
  std::fill_n(prediction_table, this->PT_len, 2); // Fill initially with value 2

  this->gbhr = 0U;

  this->num_predictions = 0;
  this->num_mispredictions = 0;
}

uint32_t BranchPredictor :: parseBranchPC(uint32_t addr) {
    uint32_t shifted = addr >> 2; // Discard the lower 2 PC bits
    uint32_t mask = (1U << m) - 1U; // Mask to get PC from m+1:2

    uint32_t PC_new = shifted & mask; // PC[m+1:2]
    
    uint32_t PC_new_n = PC_new >> (m-n);  // Top 'n' bits of PC_new
    uint32_t PC_new_m_n = PC_new & ((1U << (m-n)) - 1U);  // Lower 'm-n' bits

    uint32_t xor_n = gbhr ^ PC_new_n; // XOR with ghbr
    return ((xor_n << (m-n)) | PC_new_m_n); // return the prediction_table index
}

uint8_t BranchPredictor :: Prediction_Value(uint32_t addr, uint32_t &PT_Index) {
  PT_Index = parseBranchPC(addr);
  return this->prediction_table[PT_Index];
}

void BranchPredictor:: Impl_Bimodal(uint32_t addr, bool outcome) {
  ++(this->num_predictions);
  uint32_t PT_Index = 0;
  uint8_t prediction = Prediction_Value(addr, PT_Index);

  if(!((prediction < 2 && !outcome) || (prediction >=2 && outcome))) {
    ++(this->num_mispredictions);
  }

  if(outcome) {
    if(this->prediction_table[PT_Index] < 3)
      ++(this->prediction_table[PT_Index]);
  }else {
    if(this->prediction_table[PT_Index] > 0)
      --(this->prediction_table[PT_Index]);
  }
}

void BranchPredictor :: Impl_Gshare(uint32_t addr, bool outcome) {

  this->Impl_Bimodal(addr, outcome);

  gbhr = (outcome << (n-1) | (gbhr >> 1));
}

void BranchPredictor :: Print_Contents() {
  std::cout << "OUTPUT" << std::endl;
  std::cout << "number of predictions:\t" << this->num_predictions << std::endl;
  std::cout << std::fixed << std::setprecision(2) << "number of mispredictions:\t" << this->num_mispredictions << std::endl;
  
  float misprediction_rate = (float)this->num_mispredictions/(float)this->num_predictions;
  std::cout << "misprediction rate:\t" << misprediction_rate*100 << "%" << std::endl;

  std::cout << "FINAL\t" << this->predictor << " CONTENTS" << std::endl;
  for(uint32_t i = 0; i < this->PT_len; ++i) {
    std::cout << i << "\t" << (unsigned)this->prediction_table[i] << std::endl;
  }
}
