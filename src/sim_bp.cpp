#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cmath>
#include <iomanip>

#include "sim_bp.h"

uint8_t *chooser_table;
uint32_t K = 0, CT_len;
BranchPredictor BP_g, BP_b;
/*  argc holds the number of command line arguments
    argv[] holds the commands themselves

    Example:-
    sim bimodal 6 gcc_trace.txt
    argc = 4
    argv[0] = "sim"
    argv[1] = "bimodal"
    argv[2] = "6"
    ... and so on
*/
int main (int argc, char* argv[]) {
    FILE *FP;               // File handler
    char *trace_file;       // Variable that holds trace file name;
    bp_params params;       // look at sim_bp.h header file for the the definition of struct bp_params
    bool outcome;           // Variable holds branch outcome, true = taken; false = not taken
    unsigned long int addr; // Variable holds the address read from input file
    BranchPredictor BP;
    
    if (!(argc == 4 || argc == 5 || argc == 7))
    {
        printf("Error: Wrong number of inputs:%d\n", argc-1);
        exit(EXIT_FAILURE);
    }
    
    params.bp_name  = argv[1];
    
    // strtoul() converts char* to unsigned long. It is included in <stdlib.h>
    if(strcmp(params.bp_name, "bimodal") == 0)              // Bimodal
    {
        if(argc != 4)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M2       = strtoul(argv[2], NULL, 10);
        trace_file      = argv[3];
        printf("COMMAND\n%s %s %lu %s\n", argv[0], params.bp_name, params.M2, trace_file);
        BP.BP_Init(params.M2, 0, "BIMODAL");
    }
    else if(strcmp(params.bp_name, "gshare") == 0)          // Gshare
    {
        if(argc != 5)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M1       = strtoul(argv[2], NULL, 10);
        params.N        = strtoul(argv[3], NULL, 10);
        trace_file      = argv[4];
        printf("COMMAND\n%s %s %lu %lu %s\n", argv[0], params.bp_name, params.M1, params.N, trace_file);
        BP.BP_Init(params.M1, params.N, "GSHARE");
    }
    else if(strcmp(params.bp_name, "hybrid") == 0)          // Hybrid
    {
        if(argc != 7)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.K        = strtoul(argv[2], NULL, 10);
        params.M1       = strtoul(argv[3], NULL, 10);
        params.N        = strtoul(argv[4], NULL, 10);
        params.M2       = strtoul(argv[5], NULL, 10);
        trace_file      = argv[6];
        printf("COMMAND\n%s %s %lu %lu %lu %lu %s\n", argv[0], params.bp_name, params.K, params.M1, params.N, params.M2, trace_file);

        K = params.K;
        CT_len = (unsigned)pow(2,K);
        chooser_table = new uint8_t[CT_len];
        std::fill_n(chooser_table, CT_len, 1); // Initial fill of 1

        BP_b.BP_Init(params.M2, 0, "BIMODAL");
        BP_g.BP_Init(params.M1, params.N, "GSHARE");
    }
    else
    {
        printf("Error: Wrong branch predictor name:%s\n", params.bp_name);
        exit(EXIT_FAILURE);
    }
    
    // Open trace_file in read mode
    FP = fopen(trace_file, "r");
    if(FP == NULL)
    {
        // Throw error and exit if fopen() failed
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }
    
    char str[2];
    while(fscanf(FP, "%lx %s", &addr, str) != EOF)
    {
        
        outcome = (str[0] == 't') ? true : false;
        if(!strcmp(params.bp_name, "bimodal")) {
            BP.Impl_Bimodal(addr, outcome);
        } else if(!strcmp(params.bp_name, "gshare")) {
            BP.Impl_Gshare(addr, outcome);
        } else if(!strcmp(params.bp_name, "hybrid")) {
            Impl_Hybrid(addr, outcome);
        }
        // if (outcome == 't')
        //     printf("%lx %s\n", addr, "t");           // Print and test if file is read correctly
        // else if (outcome == 'n')
        //     printf("%lx %s\n", addr, "n");          // Print and test if file is read correctly
        /*************************************
            Add branch predictor code here
        **************************************/
    }
    if(strcmp(params.bp_name, "hybrid"))
        BP.Print_Contents();
    else {
        std::cout << "OUTPUT" << std::endl;

        uint32_t num_predictions = (BP_b.num_predictions + BP_g.num_predictions);
        uint32_t num_mispredictions = (BP_b.num_mispredictions + BP_g.num_mispredictions);

        std::cout << "number of predictions:\t" << num_predictions << std::endl;
        std::cout << "number of mispredictions:\t" << num_mispredictions << std::endl;
        
        float misprediction_rate = (float)num_mispredictions/(float)num_predictions;
        std::cout << std::fixed << std::setprecision(2) << "misprediction rate:\t" << misprediction_rate*100 << "%" << std::endl;

        std::cout << "FINAL\t" << "CHOOSER CONTENTS" << std::endl;
        for(uint32_t i = 0; i < CT_len; ++i) {
            std::cout << i << "\t" << (unsigned)chooser_table[i] << std::endl;
        }

        std::cout << "FINAL\t" << "GSHARE CONTENTS" << std::endl;
        for(uint32_t i = 0; i < BP_g.PT_len; ++i) {
            std::cout << i << "\t" << (unsigned)BP_g.prediction_table[i] << std::endl;
        }

        std::cout << "FINAL\t" << "BIMODAL CONTENTS" << std::endl;
        for(uint32_t i = 0; i < BP_b.PT_len; ++i) {
            std::cout << i << "\t" << (unsigned)BP_b.prediction_table[i] << std::endl;
        }
    }
    return 0;
}
