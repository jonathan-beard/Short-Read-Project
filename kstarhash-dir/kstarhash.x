#include "nucformat.h"

block kstarhash{
   input    UNSIGNED8 x0;
   output   array<UNSIGNED8>[KSTRUCTSIZE]  y0;
};
platform C{
   impl kstarhash (base="kstarhash");
};
