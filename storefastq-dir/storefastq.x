
block storefastq {
   input    UNSIGNED8 x0;
   config   STRING    filename = "fastqout.dat";
};

platform C {
   impl storefastq (base="storefastq");
};

