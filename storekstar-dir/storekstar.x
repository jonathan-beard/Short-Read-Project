#ifndef KSTRUCTSIZE
#define KSTRUCTSIZE 14
#endif

block storekstar {
   input  array<UNSIGNED8>[KSTRUCTSIZE] x0;
   config STRING filename = "kstar_out.dat";
};

platform C {
   impl storekstar (base="storekstar");
};

