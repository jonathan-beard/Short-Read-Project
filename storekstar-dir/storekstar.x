
block storekstar {
   input  UNSIGNED8 x0;
   config STRING filename = "kstar_out.dat";
};

platform C {
   impl storekstar (base="storekstar");
};

