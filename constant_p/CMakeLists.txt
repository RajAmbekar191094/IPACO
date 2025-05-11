
add_llvm_library( constant_p MODULE BUILDTREE_ONLY
constant_p.cpp
PLUGIN_TOOL
  opt
  )
