#ifndef ANALYSIS_DATA_H
#define ANALYSIS_DATA_H

#include "llvm/IR/Function.h"
#include <unordered_map>
#include <string>

// Declare the map as extern
using namespace llvm;
extern std::unordered_map<llvm::Function*, std::unordered_map<std::string, std::string>> call_site_data;
extern std::unordered_map<llvm::Function*, std::unordered_map<std::string, std::pair<std::string,int>>> Function_constants_data;
extern std::queue<llvm::Function*> Visited;
extern void runIntraProceduralCP(llvm::Function &F);

extern std::string processString(const std::string& input) ;
extern std::string& ltrim(std::string& s, const char* t);
extern int safe_stoi(const std::string& str);




#endif // ANALYSIS_DATA_H
