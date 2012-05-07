#ifndef KSTRUCTSIZE
#define KSTRUCTSIZE 14 
#endif

block kstarhash{
   input    UNSIGNED8 x0;
   output   array<UNSIGNED8>[KSTRUCTSIZE]  y0;
};
platform C{
   impl kstarhash (base="kstarhash");
};
