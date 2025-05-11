add_llvm_library(cons_eval MODULE BUILDTREE_ONLY
cons_eval.cpp
intra_proc.cpp

  DEPENDS
  intrinsics_gen
  PLUGIN_TOOL
  opt
  )
