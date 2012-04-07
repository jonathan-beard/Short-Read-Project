block parsefastq{
   output UNSIGNED8 y0; /* Read struct */
   output UNSIGNED8 y1; /* Read struct */
   config STRING filename = "test.fnq";
};
platform C{
   impl parsefastq (base="parsefastq");
};
