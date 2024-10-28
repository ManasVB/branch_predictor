#include<iostream>
#include <cstring>
#include <cmath>
#include "sim_bp.h"

using namespace std;

void BranchPredictor :: BP_Init(uint32_t m, uint32_t n) {
  this->m = m;
  this->n = n;
  this->PT_len = (unsigned)pow(2,m);

  prediction_table = new uint8_t[PT_len];
  std::fill_n(prediction_table, this->PT_len, 2); // Fill initially with value 2

  this->num_predictions = 0;
  this->num_mispredictions = 0;
}

uint32_t BranchPredictor :: parseBranchPC(uint32_t addr, uint32_t m) {
    uint32_t shifted = addr >> 2; // Discard the lower 2 PC bits
    uint32_t mask = (1U << (m-1)) - 1U;
    return (shifted & mask);
}

void BranchPredictor:: Impl_Bimodal(uint32_t addr, bool outcome) {
  ++(this->num_predictions);
  uint32_t PT_Index = parseBranchPC(addr, this->m);
  uint8_t prediction = this->prediction_table[PT_Index];

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

void BranchPredictor :: Print_Contents() {
  std::cout << "OUTPUT" << std::endl;
  std::cout << "number of predictions:" << this->num_predictions << std::endl;
  std::cout << "number of predictions:" << this->num_mispredictions << std::endl;
  
  float misprediction_rate = this->num_mispredictions/this->num_predictions;
  std::cout << "misprediction rate:" << misprediction_rate << std::endl;

  std::cout << "FINAL\t" << "BIMODAL CONTENTS" << std::endl;
  for(uint32_t i = 0; i < this->PT_len; ++i) {
    std::cout << i << "\t" << (unsigned)this->prediction_table[i] << std::endl;
  }
}
