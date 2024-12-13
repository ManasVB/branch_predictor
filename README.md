## Dynamic Branch Predictor

- Developed a sophisticated Dynamic Branch Prediction Simulator for Bimodal predictor (with a Branch History Table - BHT), Gshare predictor (with a global branch history register), and Hybrid predictor (Bimodal + Gshare) models.
- Analyzed the predictability of different control flow behaviors across a set of workload traces from the SPECint95 benchmark suite.

1. Type "make" to build.  (Type "make clean" first if you already compiled and want to recompile from scratch.)

2. Run trace reader:

   To run without throttling output:
   ./sim bimodal 6 gcc_trace.txt
   ./sim gshare 9 3 gcc_trace.txt
   ./sim hybrid 8 14 10 5 gcc_trace.txt

   To run with throttling (via "less"):
   ./sim bimodal 6 gcc_trace.txt | less
   ./sim gshare 9 3 gcc_trace.txt | less
   ./sim hybrid 8 14 10 5 gcc_trace.txt | less
