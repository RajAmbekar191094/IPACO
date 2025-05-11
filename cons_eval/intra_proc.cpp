#include "llvm/Pass.h"
#include "llvm/IR/Module.h" 
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instructions.h" 
#include "llvm/IR/LegacyPassManager.h"
#include <vector>
#include <fstream>
#include "llvm/IR/Constants.h"
#include <unordered_map>
#include <string>
#include <queue>
#include <unordered_set>
#include <stdexcept>
#include <sstream>
#include "intra_proc.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include <iostream>




// Your code here
using namespace llvm;
 extern std::unordered_map<llvm::Function*, std::unordered_map<std::string, std::string>> call_site_data;
 extern std::queue<llvm::Function*> Visited;
std::unordered_map<llvm::Function*, std::unordered_map<std::string, std::pair<std::string, int>>> Function_constants_data;
std::unordered_map<BasicBlock*, std::unordered_map<std::string, std::string>> BlockVarValueMaps;
unsigned int lineNumber = 0;
std::string removeSpaces(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (c != ' ') {
            result += c;
        }
    }
    return result;
}
bool compare_maps(const std::unordered_map<std::string, std::string>& map1, const std::unordered_map<std::string, std::string>& map2) {
// If the sizes of the maps are different, they cannot be equal
if (map1.size() != map2.size()) {
    return false;
}

// Iterate over elements of map1
for (const auto& pair : map1) {
    // Check if the key exists in map2 and if its corresponding value is the same
    auto it = map2.find(pair.first);
    if (it == map2.end() || it->second != pair.second) {
        return false;
    }
}

// Iterate over elements of map2 to ensure all keys are present in map1 (already checked) and have same values
for (const auto& pair : map2) {
    auto it = map1.find(pair.first);
    if (it == map1.end() || it->second != pair.second) {
        return false;
    }
}

// Maps are equal
return true;
}


// Function to take the meet of argument values
std::unordered_map<std::string, std::string> takeMeetofArguments(std::unordered_map<std::string, std::string>& prevArgs, const std::unordered_map<std::string, std::string>& newArgs) {
    std::unordered_map<std::string, std::string>map_after_meet=newArgs;
    for (auto& [arg, value] : newArgs) {
        if (prevArgs.find(arg) != prevArgs.end() && prevArgs[arg] != value) {
            // value = /* Bottom value */;
             map_after_meet[arg] ="BOTTOM";
        }
    }
    return map_after_meet;
}


void printMap(const std::unordered_map<std::string, std::string>& varValueMap) {
    // outs() << "VarValueMap:\n";
    for (const auto& pair : varValueMap) {
       outs() << pair.first << ": " << pair.second << "\n";
    }
}


std::vector<std::string> results;
int safe_stoi(const std::string& str) {
    std::istringstream iss(str);
    int result;
    if (!(iss >> result)) {
        // Handle invalid input
        return 0; // Or any other appropriate value
    }
    return result;
}
// void exportResultsToFile(const std::string& filename) {
//     std::ofstream outputFile(filename);
//     if (outputFile.is_open()) {
//       for (const auto& line : results) {
//         outputFile << line << "\n";
        
//       }
//       outputFile.close();
//       errs() << "Results exported to " << filename << "\n";
//     } else {
//       errs() << "Unable to open file: " << filename << "\n";
//     }
//   }

std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

std::string processString(const std::string& input) {
    // Check if the input string starts with "i32 "
    if (input.find("i32") == 0) {
        // If yes, remove "i32 " and space from the beginning
        std::string result = input.substr(3); // Remove "i32 " from the beginning
        ltrim(result);
        return result;
    } else {
        // If not, return the original string
        // ltrim(result);
        return input;
    }
}
std::string evaluateBinaryOperation(const std::string& operand1, const std::string& operand2, const std::string& operation) {
    if (operand1 == "BOTTOM" || operand2 == "BOTTOM") {
        return "BOTTOM"; // If any operand is BOTTOM, result is BOTTOM
    } else if (operand1 == "TOP" || operand2 == "TOP") {
        return "TOP"; // If any operand is TOP, result is TOP
    }
    if (operation == "add") {
        
        return std::to_string(safe_stoi(operand1) + safe_stoi(operand2));
    } else if (operation == "sub") {
        return std::to_string(safe_stoi(operand1) - safe_stoi(operand2));
    } else if (operation == "mul") {
        return std::to_string(safe_stoi(operand1) * safe_stoi(operand2));
    } else if (operation == "sdiv") {
        if (std::stoi(operand2) == 0) {
        outs()<<("Division by zero error");
        }
        return std::to_string(safe_stoi(operand1) / safe_stoi(operand2));
    } else if (operation == "srem") {
        if (std::stoi(operand2) == 0) {
            outs()<<("Division by zero error");
        }
        return std::to_string(safe_stoi(operand1) % safe_stoi(operand2));
    } else if (operation == "icmp_eq") {
        // Handle integer comparisons (e.g., eq, ne, sgt, slt, etc.)
        // Implement comparison logic and return result as "0" or "1"
        int op1_int = safe_stoi(operand1);
        int op2_int = safe_stoi(operand2);
        int result = (op1_int == op2_int)? 1:0;
        return std::to_string(result);
    } 
    else if (operation == "icmp_ne") {
        // Handle integer comparisons (e.g., eq, ne, sgt, slt, etc.)
        // Implement comparison logic and return result as "0" or "1"
        int op1_int = safe_stoi(operand1);
        int op2_int = safe_stoi(operand2);
        int result = (op1_int != op2_int)? 1:0;
        return std::to_string(result);
    } 
    else if (operation == "icmp_sgt") {
        // Handle integer comparisons (e.g., eq, ne, sgt, slt, etc.)
        // Implement comparison logic and return result as "0" or "1"
        int op1_int = safe_stoi(operand1);
        int op2_int = safe_stoi(operand2);
        int result = (op1_int > op2_int)? 1:0;
        return std::to_string(result);
    } 
    else if (operation == "icmp_slt") {
        // Handle integer comparisons (e.g., eq, ne, sgt, slt, etc.)
        // Implement comparison logic and return result as "0" or "1"
        int op1_int = safe_stoi(operand1);
        int op2_int = safe_stoi(operand2);
        int result = (op1_int < op2_int)? 1:0;
        return std::to_string(result);
    } 
    else if (operation == "icmp_sge") {
        // Handle integer comparisons (e.g., eq, ne, sgt, slt, etc.)
        // Implement comparison logic and return result as "0" or "1"
        int op1_int = safe_stoi(operand1);
        int op2_int = safe_stoi(operand2);
        int result = (op1_int >= op2_int)? 1:0;
        return std::to_string(result);
    } 
    else if (operation == "icmp_sle") {
        // Handle integer comparisons (e.g., eq, ne, sgt, slt, etc.)
        // Implement comparison logic and return result as "0" or "1"
        int op1_int = safe_stoi(operand1);
        int op2_int = safe_stoi(operand2);
        int result = (op1_int <= op2_int)? 1:0;
        return std::to_string(result);
    } 
    else if (operation == "icmp_ugt") {
        // Handle integer comparisons (e.g., eq, ne, sgt, slt, etc.)
        // Implement comparison logic and return result as "0" or "1"
        int op1_int = safe_stoi(operand1);
        int op2_int = safe_stoi(operand2);
        int result = (op1_int > op2_int)? 1:0;
        return std::to_string(result);
    } 
    else if (operation == "icmp_ult") {
        // Handle integer comparisons (e.g., eq, ne, sgt, slt, etc.)
        // Implement comparison logic and return result as "0" or "1"
        int op1_int = safe_stoi(operand1);
        int op2_int = safe_stoi(operand2);
        int result = (op1_int < op2_int)? 1:0;
        return std::to_string(result);
    } else if (operation == "icmp_uge") {
        // Handle integer comparisons (e.g., eq, ne, sgt, slt, etc.)
        // Implement comparison logic and return result as "0" or "1"
        int op1_int = safe_stoi(operand1);
        int op2_int = safe_stoi(operand2);
        int result = (op1_int >= op2_int)? 1:0;
        return std::to_string(result);
    } 
    else if (operation == "icmp_ule") {
        // Handle integer comparisons (e.g., eq, ne, sgt, slt, etc.)
        // Implement comparison logic and return result as "0" or "1"
        int op1_int = safe_stoi(operand1);
        int op2_int = safe_stoi(operand2);
        int result = (op1_int <= op2_int)? 1:0;
        return std::to_string(result);
    } 
    else {
        
        errs()<<("unsupported op");
        return NULL;
    }
    return NULL;
}

std::string meet_operation(const std::vector<std::string>& predValues) {

    std::string commonValue = predValues[0];
    bool hasTOP = false;
    bool hasNonSame = false;
    
    // Iterate through all values starting from the second one
    for (size_t i = 1; i < predValues.size(); ++i) {
       
        if (predValues[i] == "TOP") {
            hasTOP = true;
          
        } else if (predValues[i] == "BOTTOM") {
            return "BOTTOM";
        } else {
            if (predValues[i] != commonValue) {
                hasNonSame = true;
                break;
            }
            
        }
    }
    if(hasNonSame){
        // outs()<<"Should return"<<"\n";
      return "BOTTOM"; 
    }
    else if(hasTOP && !hasNonSame){
      return "TOP";
    }
    else{
        //  outs()<<"Should not return"<<"\n";
      return commonValue;
    }
    }
std::string predicateToString(CmpInst::Predicate pred) {
    switch (pred) {
        case CmpInst::ICMP_EQ: return "eq";
        case CmpInst::ICMP_NE: return "ne";
        case CmpInst::ICMP_UGT: return "ugt";
        case CmpInst::ICMP_UGE: return "uge";
        case CmpInst::ICMP_ULT: return "ult";
        case CmpInst::ICMP_ULE: return "ule";
        case CmpInst::ICMP_SGT: return "sgt";
        case CmpInst::ICMP_SGE: return "sge";
        case CmpInst::ICMP_SLT: return "slt";
        case CmpInst::ICMP_SLE: return "sle";
        default: return "unknown";
    }
}

std::string get_predecessor_values_and_meet(BasicBlock *BB, Function &F,std::string value) {
    bool found = false;
                    // Check if the current block has multiple predecessors
                    
                        std::vector<std::string> predValues;
                        // Iterate through all predecessors
                        for (auto *Pred : predecessors(BB)) {
                            // Access the map of the predecessor block
                            auto it = BlockVarValueMaps.find(Pred);
                            
                            if (it != BlockVarValueMaps.end()) {
                                // Check if the value operand exists in the predecessor's map
                                if (it->second.find(value) != it->second.end()) {
                                    // Update VarValueMap with the value from the predecessor's map
                                    predValues.push_back(it->second[value]);
                                    found = true;
                                
                    // Function_constants_data[BB->getParent()][alloca_string] = std::make_pair("TOP", lineNumber);            }
                            }
                        }
                        }
                   
                        if (found) {
                            // If the value operand is found in predecessor maps
                            // Pass all the values to a take_meet function along with BlockVarValueMaps
                            std::string meet_result = meet_operation(predValues);
                            return meet_result;
                            
                        }
                        else{
                             return "TOP";
                        }
                        

    
}
    

std::unordered_map<std::string, std::string> processBasicBlock_write_results(BasicBlock *BB, Function &F,std::unordered_map<std::string, std::string>& VarValueMap) {
    std::unordered_map<std::string,std::string>local_map;
    
     outs()<<F.getName().str()<<"\n";
      for (auto &Inst : *BB) {
    //    lineNumber++;
        if (DILocation *Loc = Inst.getDebugLoc()){
            //  outs()<<"************************"<<"\n";
            lineNumber = Loc->getLine();
        }
         if (isa<AllocaInst>(Inst)) {


          std::string instructionString;
          raw_string_ostream os(instructionString);
          Inst.print(os); // Print the instruction to a string
          
          AllocaInst *Alloca = cast<AllocaInst>(&Inst);
          std::string lhs= os.str();
          std::string alloca_string;
           raw_string_ostream (alloca_string) << *Alloca;
           alloca_string=alloca_string.substr(0, alloca_string.find("="));
           alloca_string.erase(std::remove_if(alloca_string.begin(), alloca_string.end(), ::isspace), alloca_string.end());
        //   lhs += "  --> " + Alloca->getName().str();
        //   ltrim(lhs);
        //   lhs += "=TOP";     
          VarValueMap[alloca_string]="TOP";  
          local_map[alloca_string]="TOP";

       
        Function_constants_data[BB->getParent()][alloca_string] = std::make_pair("TOP", lineNumber);
        //   results.push_back(lhs);
          } 
        
        if (isa<StoreInst>(&Inst)) {
                 unsigned int lineNumber = 0;
                if (DILocation *Loc = Inst.getDebugLoc()){
                    lineNumber = Loc->getLine();
                }
                StoreInst *Store = cast<StoreInst>(&Inst);
                Value *ValueOperand = Store->getValueOperand();
                Value *PointerOperand = Store->getPointerOperand();
                 std::string value_string;
                 std::string pointer_string;
               
                raw_string_ostream (value_string) <<*ValueOperand;
				raw_string_ostream (pointer_string) << *PointerOperand;
                // outs()<<"Not loaded but parameter:"<<value_string<<"\n";
                value_string=value_string.substr(0, value_string.find("="));
                pointer_string=pointer_string.substr(0, pointer_string.find("="));
               value_string.erase(std::remove_if(value_string.begin(), value_string.end(), ::isspace), value_string.end());
                pointer_string.erase(std::remove_if(pointer_string.begin(), pointer_string.end(), ::isspace), pointer_string.end());
                value_string=processString(value_string);
                pointer_string=processString(pointer_string);
                
                 

                if (ConstantInt *CI = dyn_cast<ConstantInt>(ValueOperand)) {
                    // If the value operand is a constant integer
                    // std::string rhs = "="+ std::to_string(CI->getSExtValue());
                    VarValueMap[pointer_string] = std::to_string(CI->getSExtValue());
                   
                    local_map[pointer_string] = std::to_string(CI->getSExtValue());
                   
                    Function_constants_data[BB->getParent()][pointer_string] = std::make_pair(std::to_string(CI->getSExtValue()), lineNumber);
                    // results.push_back(lhs+rhs);
                } else {
                    // If the value operand is not a constant integer but a variable
                     if (local_map.find(value_string) != local_map.end()){ 
                        // &&local_map[value_string] != "TOP" ) {
                        // If the value operand is already present in the current VarValueMap and its value is not TOP 
                      
                        VarValueMap[pointer_string] = local_map[value_string];
                         Function_constants_data[BB->getParent()][pointer_string] = std::make_pair(local_map[value_string], lineNumber);
                        //  std::string rhs = " = " + VarValueMap[pointer_string]+ "," + value_string +"="+VarValueMap[pointer_string];
                        //  results.push_back(lhs + rhs);
                    } 
                    else if (local_map[value_string] == "BOTTOM"){
                        VarValueMap[pointer_string]="BOTTOM";
                          Function_constants_data[BB->getParent()][pointer_string] = std::make_pair("BOTTOM", lineNumber);
                        // VarValueMap[pointer_string] = local_map[value_string];
                        //  std::string rhs = " = " + VarValueMap[pointer_string]+ "," + value_string +"="+VarValueMap[pointer_string];
                        //  results.push_back(lhs + rhs);
                    }
                    else {
                        // bool found = false;
                         // Check if the current block has multiple predecessors
                        if (BB->getSinglePredecessor() == nullptr &&  BB != &BB->getParent()->getEntryBlock()) {

                            if(pointer_string=="%i.addr" && value_string=="%i"){
                                outs()<<"MULTI-PRED!!"<<"\n";
                                outs()<<VarValueMap[value_string]<<"\n";
                            }
                        
                         VarValueMap[pointer_string]=get_predecessor_values_and_meet(BB,F,value_string);
                         local_map[pointer_string]=VarValueMap[value_string];
                         Function_constants_data[BB->getParent()][pointer_string] = std::make_pair(VarValueMap[value_string], lineNumber);
                        //   std::string rhs = " = " + VarValueMap[pointer_string]+ "," + value_string+"="+VarValueMap[pointer_string];
                        //  results.push_back(lhs + rhs);
                        }
                        else {
                            //If the current block has only one predecessor, proceed as usual
                            if(pointer_string=="%i.addr" && value_string=="%i"){
                                outs()<<"SINGLE_PRED!!"<<"\n";
                                 outs()<<VarValueMap[pointer_string]<<"\n";
                                outs()<<VarValueMap[value_string]<<"\n";
                                std::string instructionString;
                                raw_string_ostream os(instructionString);
                                Inst.print(os); // Print the instruction to a string
                                outs()<<os.str()<<"\n";
                            }
                             
                            
                              VarValueMap[pointer_string] = VarValueMap[value_string];
                              local_map[pointer_string]=VarValueMap[value_string];
                              Function_constants_data[BB->getParent()][pointer_string] = std::make_pair(VarValueMap[value_string], lineNumber);
                            // std::string rhs = " = " + VarValueMap[pointer_string]+ "," + value_string +"="+VarValueMap[pointer_string];
                            // results.push_back(lhs + rhs);
                        }
                    }
                }
            }


        else if (BinaryOperator *BinOp = dyn_cast<BinaryOperator>(&Inst)) {
            
            Value *Op1 = BinOp->getOperand(0);
            Value *Op2 = BinOp->getOperand(1);
            std::string operation= llvm::Instruction::getOpcodeName(Inst.getOpcode());

  

                // Get all operands in string format
                std::string op1String;
                raw_string_ostream (op1String) << *Op1;
            
                
                std::string op1 =op1String;
                ltrim(op1);
                op1=op1.substr(0, op1.find("="));
            
            

            std::string op2String;
                raw_string_ostream (op2String) << *Op2;
                // Op2->print(os2);
            
                std::string op2 = op2String;
                ltrim(op2);
                op2=op2.substr(0, op2.find("="));

            std::string dest_string;
                raw_string_ostream (dest_string) << *BinOp;
                ltrim(dest_string);
                //  outs()<<"BIN_DESTINATION:"<<dest_string<<"\n";
                dest_string=dest_string.substr(0, dest_string.find("="));
                op1.erase(std::remove_if(op1.begin(), op1.end(), ::isspace), op1.end());
                op2.erase(std::remove_if(op2.begin(), op2.end(), ::isspace), op2.end());
                dest_string.erase(std::remove_if(dest_string.begin(), dest_string.end(), ::isspace), dest_string.end());


            std::string result;
            std::string instructionString;
            raw_string_ostream os(instructionString);
            Inst.print(os); // Print the instruction to a string
            std::string lhs = os.str();
            lhs += "  --> " +dest_string;
            ltrim(lhs);

            std::string rhs;

            if (ConstantInt *CI1 = dyn_cast<ConstantInt>(Op1)) {
                // If the first operand is a constant integer
                if (ConstantInt *CI2 = dyn_cast<ConstantInt>(Op2)) {
                    // If both operands are constant integers
                    // Perform the binary operation and store the resultassignment-2/assignment-2-constant-propagation-RajAmbekar191094/assign
                    std::string val1 = std::to_string(CI1->getSExtValue());
                    std::string val2 = std::to_string(CI2->getSExtValue());
                    // std::string res;
                    
                    result= evaluateBinaryOperation(val1,val2,operation);
                    VarValueMap[dest_string] = result;
                     Function_constants_data[BB->getParent()][dest_string] = std::make_pair(result, lineNumber);
                    // rhs = result;
                    // result = std::to_string(res);
                } else {
                    // If the second operand is a variable
                    if (local_map.find(op2) != local_map.end()) {
                        // If the variable exists in VarValueMap, apply meet operation
                        result = evaluateBinaryOperation(std::to_string(CI1->getSExtValue()), local_map[op2],operation);
                        VarValueMap[dest_string] = result;
                        local_map[dest_string]=result;
                         Function_constants_data[BB->getParent()][dest_string] = std::make_pair(result, lineNumber);
                        rhs = result+", "+op2+"="+VarValueMap[op2];
                    } else {
                        // If the variable does not exist in local_map, it comes from predecessors. take meet
                            //   bool found = false;
                            // Check if the current block has multiple predecessors
                        if (BB->getSinglePredecessor() == nullptr &&  BB != &BB->getParent()->getEntryBlock()) {
                           
                        std::string op2_result=get_predecessor_values_and_meet(BB,F,op2);
                    
                        VarValueMap[op2] = op2_result;
                        local_map[op2]= op2_result;
                        
                        result=evaluateBinaryOperation(std::to_string(CI1->getSExtValue()),op2_result,operation);
                        VarValueMap[dest_string] = result;
                        local_map[dest_string]= result;
                         Function_constants_data[BB->getParent()][dest_string] = std::make_pair(result, lineNumber);
                        // rhs = result+", "+op2+"="+VarValueMap[op2];
                        }
                        else{
                            result=evaluateBinaryOperation(std::to_string(CI1->getSExtValue()),VarValueMap[op2],operation);
                            // rhs = result+", "+op2+"="+VarValueMap[op2];
                             VarValueMap[dest_string] = result;
                            local_map[dest_string]= result;
                            Function_constants_data[BB->getParent()][dest_string] = std::make_pair(result, lineNumber);
                        }
                    //             } else {
                    //                 // If the variable is not found in the predecessor blocks' maps
                    //                 // Store the variable name without its value i.e TOP
                    //                 VarValueMap[op2] = "TOP";
                    //             }
                    // }
                }
                }} 
                // If the first operand is a variable and second is constant
                
                
            else if (ConstantInt *CI2 = dyn_cast<ConstantInt>(Op2)) {
                    // If the second operand is a constant integer
                    // Apply meet operation
                        if (local_map.find(op1) != local_map.end()){
                        // If the variable exists in VarValueMap, apply meet operation
                        result = evaluateBinaryOperation(local_map[op1],std::to_string(CI2->getSExtValue()),operation);
                        VarValueMap[dest_string] = result;
                        local_map[dest_string]=result;
                         Function_constants_data[BB->getParent()][dest_string] = std::make_pair(result, lineNumber);
                        // rhs = result+", "+op1+"="+VarValueMap[op1];
                        }
                        else{
                            // Check if the current block has multiple predecessors
                        if (BB->getSinglePredecessor() == nullptr &&  BB != &BB->getParent()->getEntryBlock()) {
                                std::string op1_result=get_predecessor_values_and_meet(BB,F,op1);
                                
                                VarValueMap[op1] = op1_result;
                                local_map[op1]= op1_result;
                                 Function_constants_data[BB->getParent()][op1] = std::make_pair(op1_result, lineNumber);
                                VarValueMap[dest_string] = evaluateBinaryOperation(op1_result,std::to_string(CI2->getSExtValue()),operation);
                                local_map[dest_string]=VarValueMap[dest_string];
                                Function_constants_data[BB->getParent()][ dest_string] = std::make_pair(VarValueMap[dest_string], lineNumber);
                                // rhs = VarValueMap[dest_string]+", "+op1+"="+VarValueMap[op1]; 
                        }
                
                        else{
                            result=evaluateBinaryOperation(VarValueMap[op1],std::to_string(CI2->getSExtValue()),operation);
                            // rhs = result+", "+op1+"="+VarValueMap[op1];
                                
                            }
                // }
                        }
                } else {
                    // If both the  operands are a variable
                    std::string op1_result,op2_result;
                    if (local_map.find(op1) != local_map.end() && local_map.find(op2) != local_map.end()){
                        result = evaluateBinaryOperation(local_map[op1],local_map[op2],operation);
                        VarValueMap[dest_string] = result;
                        Function_constants_data[BB->getParent()][dest_string] = std::make_pair(result, lineNumber);
                        VarValueMap[op1] = local_map[op1];
                         Function_constants_data[BB->getParent()][  op1] = std::make_pair(local_map[op1], lineNumber);
                        VarValueMap[op2] = local_map[op2];
                         Function_constants_data[BB->getParent()][  op2 ] = std::make_pair(local_map[op2], lineNumber);
                        local_map[dest_string]=result;
                         Function_constants_data[BB->getParent()][dest_string] = std::make_pair(result, lineNumber);
                        // rhs = VarValueMap[dest_string]+", "+op1+"="+VarValueMap[op1]+" ,"+op2+"="+VarValueMap[op2];

                }
                    if (local_map.find(op1) == local_map.end() && local_map.find(op2) != local_map.end()){
                        if (BB->getSinglePredecessor() == nullptr) {
                        op1_result=get_predecessor_values_and_meet(BB,F,op1);
                        }else{
                            op1_result=VarValueMap[op1];
                        }
                        result = evaluateBinaryOperation(op1_result,local_map[op2],operation);
                        VarValueMap[op1]=op1_result;
                        local_map[op1]=op1_result;
                         Function_constants_data[BB->getParent()][  op1] = std::make_pair(local_map[op1], lineNumber);
                        VarValueMap[dest_string] = result;
                        local_map[dest_string]=result;
                         Function_constants_data[BB->getParent()][dest_string] = std::make_pair(result, lineNumber);
                        //  Function_constants_data[BB->getParent()][ local_map[dest_string]] = std::make_pair(result, lineNumber);
                        // rhs = VarValueMap[dest_string]+", "+op1+"="+VarValueMap[op1]+" ,"+op2+"="+VarValueMap[op2];

                }
                    if (local_map.find(op1) != local_map.end() && local_map.find(op2) == local_map.end()){
                        if (BB->getSinglePredecessor() == nullptr &&  BB != &BB->getParent()->getEntryBlock()) {
                        op2_result=get_predecessor_values_and_meet(BB,F,op2);
                        }
                        else{
                            op2_result=VarValueMap[op2];
                        }
                        result = evaluateBinaryOperation(local_map[op1],op2_result,operation);
                        VarValueMap[op2]=op2_result;
                        local_map[op2]=op2_result;
                         Function_constants_data[BB->getParent()][op2] = std::make_pair(op2_result, lineNumber);
                        VarValueMap[dest_string] = result;
                        local_map[dest_string]=result;
                        Function_constants_data[BB->getParent()][dest_string] = std::make_pair(result, lineNumber);
                        // rhs = VarValueMap[dest_string]+", "+op1+"="+VarValueMap[op1]+" ,"+op2+"="+VarValueMap[op2];

                }
                    if (local_map.find(op1) == local_map.end() && local_map.find(op2) == local_map.end()){
                        if (BB->getSinglePredecessor() == nullptr &&  BB != &BB->getParent()->getEntryBlock()) {
                        bool op1Found = false, op2Found = false;
                        std::vector<std::string> predMaps1;
                        std::vector<std::string> predMaps2;
                        std::vector<std::string> dest_operand;
                        // Check each predecessor map for the presence of operands
                        for (auto *Pred : predecessors(BB)) {
                            auto it = BlockVarValueMaps.find(Pred);
                            if (it != BlockVarValueMaps.end()) {
                                if (it->second.find(op1) != it->second.end()) {
                                    op1Found = true;
                                    predMaps1.push_back(it->second[op1]);
                                }
                                if (it->second.find(op2) != it->second.end()) {
                                    op2Found = true;
                                    predMaps2.push_back(it->second[op2]);
                                }
                                if(op1Found && op2Found){
                                std::string dest_val=evaluateBinaryOperation(it->second[op1],it->second[op2],operation) ;
                                dest_operand.push_back(dest_val);


                                }
                            }
                        }
                        VarValueMap[op1]=meet_operation(predMaps1);
                        VarValueMap[op2]=meet_operation(predMaps2);
                        VarValueMap[dest_string] = meet_operation(dest_operand);
                        local_map[op1]=meet_operation(predMaps1);
                        local_map[op2]=meet_operation(predMaps2);
                        local_map[dest_string] = meet_operation(dest_operand);
                        Function_constants_data[BB->getParent()][op1] = std::make_pair(meet_operation(predMaps1), lineNumber);
                        Function_constants_data[BB->getParent()][op2] = std::make_pair(meet_operation(predMaps2), lineNumber);
                        Function_constants_data[BB->getParent()][dest_string] = std::make_pair( meet_operation(dest_operand), lineNumber);
                        // rhs = VarValueMap[dest_string]+", "+op1+"="+VarValueMap[op1]+" ,"+op2+"="+VarValueMap[op2];

                        }
                        else{
                        result=evaluateBinaryOperation(VarValueMap[op1],VarValueMap[op2],operation);
                    
                        local_map[op1]=VarValueMap[op1];
                        local_map[op2]=VarValueMap[op2];
                        local_map[dest_string] = VarValueMap[op2];
                        Function_constants_data[BB->getParent()][op1] = std::make_pair(VarValueMap[op1], lineNumber);
                        Function_constants_data[BB->getParent()][op2] = std::make_pair(VarValueMap[op2], lineNumber);
                        Function_constants_data[BB->getParent()][dest_string] = std::make_pair( VarValueMap[op2], lineNumber);
                        // rhs = VarValueMap[dest_string]+", "+op1+"="+VarValueMap[op1]+" ,"+op2+"="+VarValueMap[op2];


                        }
                }

                }
             results.push_back(lhs + "=" + rhs);
            }

            


            else if (LoadInst *Load = dyn_cast<LoadInst>(&Inst)) {

              
                //get operands in string format
                Value *PtrOperand = Load->getPointerOperand();
             
                std::string instructionString;
                raw_string_ostream os(instructionString);
                Inst.print(os); // Print the instruction to a string
                std::string lhs = os.str();
                ltrim(lhs);
                lhs += "  --> ";
                std::string ptr_string,val_string;
                raw_string_ostream (ptr_string) << *PtrOperand;
               
            
                val_string=lhs.substr(0, lhs.find("="));
                ptr_string=ptr_string.substr(0, ptr_string.find("="));
                 val_string.erase(std::remove_if(val_string.begin(), val_string.end(), ::isspace), val_string.end());
                 ptr_string.erase(std::remove_if(ptr_string.begin(), ptr_string.end(), ::isspace), ptr_string.end());
                
 
                if (local_map.find(ptr_string) != local_map.end() &&
                    local_map[ptr_string] != "TOP"){
                    // If the pointer operand is already present in the current VarValueMap and its value is not TOP or BOTTOM
                     local_map[val_string] = local_map[ptr_string];
                    VarValueMap[val_string] = local_map[ptr_string];
                     Function_constants_data[BB->getParent()][val_string] = std::make_pair(local_map[ptr_string], lineNumber);
                    //  std::string rhs= val_string+"="+VarValueMap[ptr_string]+ ", " +ptr_string+"="+VarValueMap[ptr_string];
                } else {
                    
                    
                    // // Check if the current block has multiple predecessors
                     std::string operand_result;
                  if (BB->getSinglePredecessor() == nullptr &&  BB != &BB->getParent()->getEntryBlock()) {
                   operand_result=get_predecessor_values_and_meet(BB,F,ptr_string);
                  }
                  else{
                    operand_result=VarValueMap[ptr_string];

                  }
                 
                    VarValueMap[val_string] = operand_result;
                    VarValueMap[ptr_string] = operand_result;
                    local_map[val_string] = operand_result;
                    local_map[ptr_string] = operand_result;
                    Function_constants_data[BB->getParent()][val_string] = std::make_pair(operand_result, lineNumber);
                     Function_constants_data[BB->getParent()][ptr_string] = std::make_pair(operand_result, lineNumber);

                }
                // std::string rhs= val_string+"="+VarValueMap[ptr_string]+ ", " +ptr_string+"="+VarValueMap[ptr_string];
                // results.push_back(lhs+rhs);
            }
                if (isa<CallInst>(&Inst)) {
                    // outs()<<F.getName().str()<<"\n";
                    if(CallInst *Call = dyn_cast<CallInst>(&Inst)){
               
                        Function *Callee = Call->getCalledFunction();
                        std::unordered_map<std::string, std::string> parameterValues;
                        // int i=0;
                        // for (const Argument &Arg : Callee->args()) {
                        //     Value *ArgOperand = Call->getArgOperand(i);
                        //     Value *CallArg = Call->getArgOperand(Arg.getArgNo());
                        //     i++;
                        //     if (ConstantInt *ConstArg = dyn_cast<ConstantInt>(CallArg)) {
                        //         // If the argument is a constant, store its value
                        //         parameterValues["%"+Arg.getName().str()] = std::to_string(ConstArg->getValue().getSExtValue());
                        //     } else {
                        //         // If the argument is not a constant, perform constant propagation to get its value
                        //         // For simplicity, assume the value is 42
                        //         // Replace this with actual constant propagation code
                        //         parameterValues["%"+Arg.getName().str()] = VarValueMap["%"+ArgOperand->getName().str()];
                        //         outs()<<"NEEDS DEBUG "<<"\n";
                        //         // outs()<<"ACTUALS:"<<"%"+ArgOperand->getName().str()<<"\n";
                        //         outs()<<"RAM PRABHU:"<<ArgOperand->getName().str()<<"\n";
                        //         printMap(VarValueMap);

                        //     }
                        // }
                        std::vector<std::string>formals;
                        std::vector<std::string>actuals;
                        outs()<<"MAP>>>>>>>>"<<"\n";
                        printMap(VarValueMap);
                        outs()<<"FORMALS>>>>>>>>"<<"\n";
                          for (const Argument &Arg : Callee->args()) {
                            // Value *ArgOperand = Call->getArgOperand(i);
                            outs()<<"%"+Arg.getName().str()<<"\n";
                            formals.push_back("%"+Arg.getName().str());
                          }
                          outs()<<"ACTUALS>>>>>>>>>"<<"\n";
                         // Get the actual arguments of the call instruction
                            // FunctionType *FTy = CalledFunction->getFunctionType();
                            for (unsigned i = 0; i < Call->getNumOperands()-1; ++i) {
                                Value *ArgValue = Call->getArgOperand(i);

                                // If argument is not an instruction, it must be a constant
                                if (Instruction *ArgInst = dyn_cast<Instruction>(ArgValue)) {
                                    // ArgValue is an instruction, you can do whatever you want with it
                                    // For example, print the instruction
                                    std::string InstString;
                            llvm::raw_string_ostream InstStream(InstString);
                               ArgInst->print(InstStream);
                               InstStream.flush();
                               
                               
                                std::string val_string = InstStream.str();
                                ltrim(val_string);
                                val_string=val_string.substr(0, val_string.find("="));
                                //  outs()<<"%"+val_string<<"\n";
                                outs()<<"MISS:"<<val_string<<"\n";
                                val_string=removeSpaces(val_string);
                                    errs() << "  Actual parameter :" << VarValueMap[val_string] << "\n";
                                    actuals.push_back(VarValueMap[val_string]);
                                } else if (auto *ConstantArg = dyn_cast<Constant>(ArgValue)){
                                    // ArgValue is not an instruction, it's a constant
                                    // Retrieve the corresponding formal parameter name from the called function's arguments
                                    // if (i < FTy->getNumParams()) {
                                    //     Argument *Arg = CalledFunction->getArg(i);
                                    //     errs() << "  Actual parameter " << i << ": Constant: " << *ArgValue << "\n";
                                      std::string InstString;
                                    llvm::raw_string_ostream InstStream(InstString);
                               ConstantArg->print(InstStream);
                               InstStream.flush();
                               
                               
                                std::string val_string = InstStream.str();
                                ltrim(val_string);
                                val_string=val_string.substr(0, val_string.find("="));
                                //  outs()<<"%"+val_string<<"\n";
                                    errs() << "  Actual parameter :" << processString(val_string) << "\n";
                                    actuals.push_back(processString(val_string));
                                    // }
                                }
                            }
                            for (unsigned i = 0; i < formals.size(); ++i) {
                                parameterValues[formals[i]]=actuals[i];
                            }
                             
                              
                            //     llvm::raw_string_ostream InstStream(InstString);
                            //    ArgInst->print(InstStream);
                            //    InstStream.flush();
                               
                               
                            //     std::string val_string = InstStream.str();
                            //     ltrim(val_string);
                            //     val_string=val_string.substr(0, val_string.find("="));
                            //      outs()<<"%"+val_string<<"\n";
                            // if (auto *ArgInst = dyn_cast<Instruction>(Call->getArgOperand(i))) {
                            //     // Operand is an instruction
                            //     std::string InstString;
                            //     llvm::raw_string_ostream InstStream(InstString);
                            //    ArgInst->print(InstStream);
                            //    InstStream.flush();
                               
                               
                            //     std::string val_string = InstStream.str();
                            //     ltrim(val_string);
                            //     val_string=val_string.substr(0, val_string.find("="));
                            //      outs()<<"%"+val_string<<"\n";
                                
                            //     //  outs()<<"Actual:"+val_string<<"Formal:"<<Callee->getArg(i-1)->getName().str()<<"\n";
                            //    parameterValues[val_string] = VarValueMap[val_string];
                            // } else if (auto *ConstArg = dyn_cast<ConstantInt>(Call->getArgOperand(i))) {
                            //     // Operand is a constant integer
                            //     parameterValues[std::to_string(i)] = std::to_string(ConstArg->getSExtValue());
                            // }
                            // outs()<<*(Call->getArgOperand(i))<<"\n";
                       
       
                        // unsigned NumFormalParams = Callee->arg_size();
                        // if (!Callee)
                        //     return; // Ignore indirect calls

                        // Create a map to store parameter values for this call instruction
                        // std::unordered_map<std::string, std::string> parameterValues;
                        //  outs()<<F.getName().str()<<":"<<Callee->getName().str()<<"\n";
                        // outs()<<"Actual parameters:"<<"\n";
                        // int i=0;
                        // for (const Argument &Arg : Callee->args()) {
                        //      Value *ArgOperand = Call->getArgOperand(i);
                        //     Value *CallArg = Call->getArgOperand(Arg.getArgNo());
                        //     if (ConstantInt *ConstArg = dyn_cast<ConstantInt>(CallArg)) {
                        //         // If the argument is a constant, store its value
                        //         parameterValues["%"+Arg.getName().str()] = std::to_string(ConstArg->getValue().getSExtValue());
                        //         outs()<<"%"+ArgOperand->getName().str()<<"\n";
                        //     } else {
                        //         // If the argument is not a constant, perform constant propagation to get its value
                        //         // For simplicity, assume the value is 42
                        //         // Replace this with actual constant propagation code
                        //         parameterValues["%"+Arg.getName().str()] = VarValueMap["%"+ArgOperand->getName().str()];
                        //          outs()<<"%"+ArgOperand->getName().str()<<"\n";
                        //     }
                        // }
                        // for (unsigned i = 0; i < Call->getNumArgOperands(); ++i) {
                        //             Value *ArgValue = Call->getArgOperand(i);

                        //             // Get the corresponding formal parameter (if it exists)
                        //             if (Constant *ConstArg = dyn_cast<Constant>(ArgValue)) {
                        //                    parameterValues["%"+Arg.getName().str()] = std::to_string(ConstArg->getValue().getSExtValue());
                        //             }
                        //             if (i < NumFormalParams) {
                        //                 Argument *FormalParam = &Callee->getArg(i);

                        //                 // Get the name of the formal parameter and the actual argument value
                        //                 std::string FormalParamName = "%" + FormalParam->getName().str();
                        //                 std::string ActualArgName = "%" + ArgValue->getName().str();

                        //                 // Print or store the names as needed
                        //                 outs() << "Formal parameter: " << FormalParamName << ", Actual argument: " << ActualArgName << "\n";
                        //             } else {
                        //                 // Handle the case where the number of actual arguments exceeds the number of formal parameters
                        //                 // This could happen in varargs functions or if the call is incorrect
                        //                 // Handle this case based on your requirements
                        //                 outs() << "No corresponding formal parameter for actual argument: " << i << "\n";
                        //             }
                        //         }

                       
                           
                            // outs()<<"Actuals:"<<"\n";
                            // for (unsigned i = 0; i < Call->getNumOperands(); ++i) {
                            //     Value *ArgOperand = Call->getArgOperand(i);
                            //     // Check if the argument operand is an instruction
                            //     if (auto *ArgInst = dyn_cast<Instruction>(ArgOperand)) {
                            //         // Get the name of the argument operand
                            //         if (ConstantInt *ConstArg = dyn_cast<ConstantInt>(ArgOperand)) {
                            //                 // If the argument is a constant, store its value
                            //                 parameterValues["%"+Callee->getArg(i)->getName().str()] = std::to_string(ConstArg->getValue().getSExtValue());
                            //                 // outs()<<"%"+ArgInst->getName().str()<<"\n";
                            //             } else {
                            //                 // If the argument is not a constant, perform constant propagation to get its value
                            //                 // For simplicity, assume the value is 42
                            //                 // Replace this with actual constant propagation code
                            //                 parameterValues["%"+Callee->getArg(i)->getName().str()] = VarValueMap["%"+ArgInst->getName().str()];
                            //                 // outs()<<"%"+ArgInst->getName().str()<<"\n";
                            //             }
                            //         std::string ArgName = ArgInst->getName().str();
                            //         // Print the name of the argument operand
                            //         llvm::outs() << ArgName << "\n";
                            //     }
                            // }


                         if(F.getName().str()=="main" ){
                                // outs()<<"DEBUGGINGGGGGGGGGGGGGGG!"<<"\n";
                                // outs()<<Callee->getName().str()<<"\n";
                                //  printMap(parameterValues);
                            }



                      

                        // Store the parameter values in the map
                        // if (Visited.find(Callee) == Visited.end()) {
                            // If function is not visited before, add it to Visited queue
                            // Visited.push(Callee);
                        // }
                        bool found = false;
                            std::queue<Function*> tempQueue = Visited; // Create a temporary queue for iteration
                            while (!tempQueue.empty()) {
                                Function* frontFunc = tempQueue.front();
                                tempQueue.pop();
                                if (frontFunc == Callee) {
                                    found = true;
                                    break;
                                }
                            }
                            if (!found) {
                                // Callee not found in Visited queue, add it
                                Visited.push(Callee);
                            }

                        // Check if function already exists in call_site_data
                        //  outs()<<F.getName().str()<<":"<<Callee->getName().str()<<"\n";
                        auto it = call_site_data.find(Callee);
                        if (it != call_site_data.end()) {
                            // If function already exists, update its argument values
                            //  if(F.getName().str()=="fun" && Callee->getName().str()=="foo"){
                            //     outs()<<"DEBUG POINT !"<<"\n";
                            //     // outs() << "call_site_data:" << "\n";
                            //     // for (const auto& entry : call_site_data) {
                            //     //     llvm::Function* func = entry.first;
                            //     //     outs() << "Function: " << func->getName().str() << "\n";
                            //     //     const auto& innerMap = entry.second;
                            //     //     for (const auto& innerEntry : innerMap) {
                            //     //         const std::string& arg = innerEntry.first;
                            //     //         const std::string& value = innerEntry.second;
                            //     //        outs() << "    Argument: " << arg << ", Value: " << value << "\n";
                            //     //     }
                            //     // }
                            //       outs()<<"************* !"<<"\n";
                            //     printMap(it->second);
                            //     outs()<<"************* !"<<"\n";
                            //       printMap(parameterValues);


                            //  }
                           
                            //  for (const auto& entry : call_site_data) {
                            //         llvm::Function* func = entry.first;
                            //         outs() << "Function: " << func->getName().str() << "\n";
                            //         const auto& innerMap = entry.second;
                            //         for (const auto& innerEntry : innerMap) {
                            //             const std::string& arg = innerEntry.first;
                            //             const std::string& value = innerEntry.second;
                            //            outs() << "    Argument: " << arg << ", Value: " << value << "\n";
                            //         }
                            //     }
                             outs()<<"MEETS HAPPENING:"<<"\n";
                             printMap(it->second);
                             outs()<<"AND"<<"\n";
                             printMap(parameterValues);
                            parameterValues=takeMeetofArguments(it->second, parameterValues);
                             call_site_data[Callee] = parameterValues;


                        // if(Callee->getName().str()=="foo"){

                        //     // outs()<<"DEBUGGING!"<<"\n";
                        //     // printMap(it->second);
                        //     // printMap(parameterValues);
                        // }
                        } else {
                            // If function doesn't exist, add it with its argument values
                            call_site_data[Callee] = parameterValues;
                           
                        }

                        //Update functiomn paramters with call sirtes values after meet

                        // for (const auto& parVal : parameterValues) {
                        //     // Fill Function_constants_data
                        //     Function_constants_data[Callee][std::to_string(parVal.first)] = std::make_pair(parVal.second, 0);
                        // }
                        // Get the number of formal parameters of the callee function
 //Update functiomn paramters with call sirtes values after meet
                        for (const auto& parVal : parameterValues) {
                            // Fill Function_constants_data
                            Function_constants_data[Callee][parVal.first] = std::make_pair(parVal.second, 0);
                        }


            }
          }
    if (isa<ICmpInst>(&Inst) ) {
            std::string instructionString;
            raw_string_ostream os(instructionString);
            Inst.print(os); // Print the instruction to a string
            std::string comp_inst= os.str();
            ltrim(comp_inst);
        
            ICmpInst *IC = dyn_cast<ICmpInst>(&Inst);
            Value *Op1 = Inst.getOperand(0);
            Value *Op2 = Inst.getOperand(1);
            
            CmpInst::Predicate predicate = IC->getPredicate();
                std::string predicateStr = predicateToString(predicate);
            //  outs()<<"ICMP PRED:"<<predicateStr<<"\n";
            std::string operation= "icmp_"+predicateStr;
            std::string result;
                std::string op1String;
            raw_string_ostream (op1String) << *Op1;
            // Op1->print(os1);
            
            std::string op1 =op1String;
            ltrim(op1);
            op1=op1.substr(0, op1.find("="));
      
           

           std::string op2String;
            raw_string_ostream (op2String) << *Op2;
            // Op2->print(os2);
           
            std::string op2 = op2String;
            ltrim(op2);
            op2=op2.substr(0, op2.find("="));
     

             std::string dest_string;
            raw_string_ostream (dest_string) << *IC;
             ltrim(dest_string);
            //  outs()<<"BIN_DESTINATION:"<<dest_string<<"\n";
             dest_string=dest_string.substr(0, dest_string.find("="));
             op1.erase(std::remove_if(op1.begin(), op1.end(), ::isspace), op1.end());
             op2.erase(std::remove_if(op2.begin(), op2.end(), ::isspace), op2.end());
             dest_string.erase(std::remove_if(dest_string.begin(), dest_string.end(), ::isspace), dest_string.end());
        if (ConstantInt *CI1 = dyn_cast<ConstantInt>(Op1)) {
            // If the first operand is a constant integer
            if (ConstantInt *CI2 = dyn_cast<ConstantInt>(Op2)) {
                // If both operands are constant integers
                // Perform the binary operation and store the result
                std::string val1 = std::to_string(CI1->getSExtValue());
                std::string val2 = std::to_string(CI2->getSExtValue());
                
                
                result= evaluateBinaryOperation(val1,val2,operation);
                 results.push_back(comp_inst += dest_string+"=" +result);
                // result = std::to_string(res);
            } else {
                // If the second operand is a variable
                if (local_map.find(op2) != local_map.end()) {
                    // If the variable exists in VarValueMap, apply meet operation
                    result = evaluateBinaryOperation(std::to_string(CI1->getSExtValue()), local_map[op2],operation);
                    VarValueMap[dest_string] = result;
                    
                     results.push_back(comp_inst += dest_string+"="+result+" ,"+op2+"="+local_map[op2]);
                } else {
                     // // Check if the current block has multiple predecessors
                   if (BB->getSinglePredecessor() == nullptr &&  BB != &BB->getParent()->getEntryBlock()) {
                    
                    std::string op2_result=get_predecessor_values_and_meet(BB,F,op2);
                
                    VarValueMap[op2] = op2_result;
                    local_map[op2]= op2_result;
                    VarValueMap[dest_string] = op2_result;
                    Function_constants_data[BB->getParent()][dest_string] = std::make_pair(op2_result, lineNumber);
                     result = evaluateBinaryOperation(std::to_string(CI1->getSExtValue()), local_map[op2],operation);
                    VarValueMap[dest_string] = result;
                     Function_constants_data[BB->getParent()][dest_string] = std::make_pair(result, lineNumber);
                     results.push_back(comp_inst += dest_string+"="+result+" ,"+op2+"="+local_map[op2]);
                   }
                   else{

                    local_map[op2]= VarValueMap[op2];
                     Function_constants_data[BB->getParent()][op2] = std::make_pair( VarValueMap[op2], lineNumber);
                     result = evaluateBinaryOperation(std::to_string(CI1->getSExtValue()), local_map[op2],operation);
                      results.push_back(comp_inst += dest_string+"="+result+" ,"+op2+"="+local_map[op2]);

                   }

            }
            
            }} 
            // If the first operand is a variable and second is constant
            
            
       else if (ConstantInt *CI2 = dyn_cast<ConstantInt>(Op2)) {
                // If the second operand is a constant integer
                // Apply meet operation
                    if (local_map.find(op1) != local_map.end()){
                    // If the variable exists in local map, no need of meet
                    result = evaluateBinaryOperation(local_map[op1],std::to_string(CI2->getSExtValue()),operation);
                    VarValueMap[dest_string] = result;
                     Function_constants_data[BB->getParent()][dest_string] = std::make_pair(result, lineNumber);
                     VarValueMap[op1] =local_map[op1];
                     Function_constants_data[BB->getParent()][op1] = std::make_pair(local_map[op1], lineNumber);
                    //  results.push_back(comp_inst += " -->" + dest_string+"="+result+" ,"+op1+"="+local_map[op1]);
          

                    }
                    else{
                         // Check if the current block has multiple predecessors
                    if (BB->getSinglePredecessor() == nullptr &&  BB != &BB->getParent()->getEntryBlock()) {
                            std::string op1_result=get_predecessor_values_and_meet(BB,F,op1);
                            
                            VarValueMap[op1] = op1_result;
                            local_map[op1]= op1_result;
                            VarValueMap[dest_string] = evaluateBinaryOperation(op1_result,std::to_string(CI2->getSExtValue()),operation);
                            local_map[dest_string]=VarValueMap[dest_string];
                             Function_constants_data[BB->getParent()][dest_string] = std::make_pair(VarValueMap[dest_string], lineNumber);
                            // results.push_back(comp_inst += " -->" + dest_string+"="+result+" ,"+op1+"="+local_map[op2]);
                    }
               
                    else{
                         result=evaluateBinaryOperation(VarValueMap[op1],std::to_string(CI2->getSExtValue()),operation);
                        results.push_back(comp_inst += " -->" + dest_string+"="+result+" ,"+op1+"="+local_map[op2]);
                            
                        }
            // }
                    }
            } else {
                // If both the  operands are a variable
                std::string op1_result,op2_result;
                if (local_map.find(op1) != local_map.end() && local_map.find(op2) != local_map.end()){
                    result = evaluateBinaryOperation(local_map[op1],local_map[op2],operation);
                    VarValueMap[dest_string] = result;
                     Function_constants_data[BB->getParent()][dest_string] = std::make_pair(result, lineNumber);
                    VarValueMap[op1] = local_map[op1];
                     Function_constants_data[BB->getParent()][op1] = std::make_pair( local_map[op1], lineNumber);
                    VarValueMap[op2] = local_map[op2];
                    Function_constants_data[BB->getParent()][op2] = std::make_pair( local_map[op2], lineNumber);
                    local_map[dest_string]=result;
                //    results.push_back(comp_inst += " -->" + dest_string+"="+result+" ,"+op1+"="+local_map[op1] +"," +op2+"="+local_map[op2]);

            }
                  if (local_map.find(op1) == local_map.end() && local_map.find(op2) != local_map.end()){
                     if (BB->getSinglePredecessor() == nullptr) {
                       op1_result=get_predecessor_values_and_meet(BB,F,op1);
                     }else{
                        op1_result=VarValueMap[op1];
                     }
                    result = evaluateBinaryOperation(op1_result,local_map[op2],operation);
                    VarValueMap[op1]=op1_result;
                    local_map[op1]=op1_result;
                     Function_constants_data[BB->getParent()][op1] = std::make_pair( op1_result, lineNumber);
                    VarValueMap[dest_string] = result;
                    local_map[dest_string]=result;
                     Function_constants_data[BB->getParent()][dest_string] = std::make_pair( result, lineNumber);
                    // results.push_back(comp_inst += " -->" + dest_string+"="+result+" ,"+op1+"="+local_map[op1] +"," +op2+"="+local_map[op2]);

            }
                  if (local_map.find(op1) != local_map.end() && local_map.find(op2) == local_map.end()){
                     if (BB->getSinglePredecessor() == nullptr &&  BB != &BB->getParent()->getEntryBlock()) {
                    op2_result=get_predecessor_values_and_meet(BB,F,op2);
                     }
                     else{
                        op2_result=VarValueMap[op2];
                     }
                    result = evaluateBinaryOperation(local_map[op1],op2_result,operation);
                    VarValueMap[op2]=op2_result;
                    local_map[op2]=op2_result;
                    VarValueMap[dest_string] = result;
                    local_map[dest_string]=result;
                     Function_constants_data[BB->getParent()][dest_string] = std::make_pair( result, lineNumber);
                    // results.push_back(comp_inst += " -->" + dest_string+"="+result+" ,"+op1+"="+local_map[op1] +"," +op2+"="+local_map[op2]);

            }
                  if (local_map.find(op1) == local_map.end() && local_map.find(op2) == local_map.end()){
                     if (BB->getSinglePredecessor() == nullptr &&  BB != &BB->getParent()->getEntryBlock()) {
                    bool op1Found = false, op2Found = false;
                    std::vector<std::string> predMaps1;
                    std::vector<std::string> predMaps2;
                    std::vector<std::string> dest_operand;
                    // Check each predecessor map for the presence of operands
                    for (auto *Pred : predecessors(BB)) {
                        auto it = BlockVarValueMaps.find(Pred);
                        if (it != BlockVarValueMaps.end()) {
                            if (it->second.find(op1) != it->second.end()) {
                                op1Found = true;
                                predMaps1.push_back(it->second[op1]);
                            }
                            if (it->second.find(op2) != it->second.end()) {
                                op2Found = true;
                                predMaps2.push_back(it->second[op2]);
                            }
                            if(op1Found && op2Found){
                            std::string dest_val=evaluateBinaryOperation(it->second[op1],it->second[op2],operation) ;
                             dest_operand.push_back(dest_val);


                            }
                        }
                    }
                      VarValueMap[op1]=meet_operation(predMaps1);
                      VarValueMap[op2]=meet_operation(predMaps2);
                      VarValueMap[dest_string] = meet_operation(dest_operand);
                      local_map[op1]=meet_operation(predMaps1);
                      local_map[op2]=meet_operation(predMaps2);
                      local_map[dest_string] = meet_operation(dest_operand);
                      Function_constants_data[BB->getParent()][op1] = std::make_pair( meet_operation(predMaps1), lineNumber);
                      Function_constants_data[BB->getParent()][op2] = std::make_pair( meet_operation(predMaps2), lineNumber);
                      Function_constants_data[BB->getParent()][dest_string] = std::make_pair( result, lineNumber);
                    //   results.push_back(comp_inst += " -->" + dest_string+"="+result+" ,"+op1+"="+local_map[op1] +"," +op2+"="+local_map[op2]);

                     }
                      else{
                        result=evaluateBinaryOperation(VarValueMap[op1],VarValueMap[op2],operation);
                  
                      local_map[op1]=VarValueMap[op1];
                      local_map[op2]=VarValueMap[op2];
                      local_map[dest_string] = result;
                       Function_constants_data[BB->getParent()][op1] = std::make_pair( VarValueMap[op1], lineNumber);
                      Function_constants_data[BB->getParent()][op2] = std::make_pair(VarValueMap[op2], lineNumber);
                      Function_constants_data[BB->getParent()][dest_string] = std::make_pair( result, lineNumber);
                    //   results.push_back(comp_inst += " -->" + dest_string+"="+result+" ,"+op1+"="+local_map[op1] +"," +op2+"="+local_map[op2]);


                      }
            }

            }
            
    }
     // Check if the instruction is a branch instruction (br)
        else if (isa<BranchInst>(&Inst)) {
                std::string instructionString;
                raw_string_ostream os(instructionString);
                Inst.print(os); // Print the instruction to a string
                std::string branch_inst= os.str();
                 ltrim(branch_inst);
                 results.push_back(branch_inst);
                 results.push_back("");
                //  outs()<<std::to_string(block_count)<<"\n";
            }
}
 

 
  // }
  return VarValueMap;
  }

void runIntraProceduralCP(Function &F) {
             
                    //  outs()<<"Running_intra"<<"\n";
                    // static unsigned int lineNumber=0;




                   
                    std::unordered_map<std::string, std::string> current_map;
                    std::unordered_map<std::string, std::string> new_map;
                    std::queue<BasicBlock *> worklist;


                     //update current map with functionconstants data
                    auto it = Function_constants_data.find(&F);
                    if (it != Function_constants_data.end()) {
                        // Iterate over the parameter-value pairs and populate the current map
                        for (const auto& parameterPair : it->second) {
                            // Extract the parameter name and value from Function_constants_data
                            // const std::string& parameterName = parameterPair.first;
                            // const std::pair<std::string, int>& parameterValue = parameterPair.second;
                            
                            // Insert the parameter-value pair into the current map
                            current_map[parameterPair.first] = parameterPair.second.first;
                        }
                    }
    
                


                //push entry block and then   all other basic blocks into a worklist
                
                
                worklist.push(&F.getEntryBlock());


                // Initialize worklist with all basic blocks except the entry block
                for (auto &BB : F) {
                        outs()<<"BAsic block:"<<BB.getName()<<"\n";
                        if (&BB == &F.getEntryBlock()){
                                continue;
                        }
                        else{  
                        worklist.push(&BB);
                        }
                    }

                    

                    do{
                
                    
                        BasicBlock *CurrBlock = worklist.front();
                        worklist.pop();
                        
                        if(CurrBlock->isEntryBlock() ){
                            if(F.getName().str()=="main"){
                            outs()<<"Processing main"<<"\n";
                            }
                       
                        new_map =processBasicBlock_write_results(CurrBlock, F,current_map);
                        outs()<<"Entry processed"<<"\n";
                    
                        BlockVarValueMaps[CurrBlock]=new_map;
                        current_map=new_map;
                    

                        }
                    
                    
                        
                        if(!CurrBlock->isEntryBlock()){
                        if (CurrBlock->getSinglePredecessor() == nullptr ){
                        //CurrBlock != &CurrBlock->getParent()->getEntryBlock()) {


                            // Multiple predecessors, use meet_and_append
                            // Iterate through predecessors
                            if(F.getName().str()=="main"){
                            outs()<<"Processing main entry block has multiple predecessaors"<<"\n";
                            }
                            std::unordered_map<std::string, std::string> unionMap;
                            for (auto *Pred : predecessors(CurrBlock)) {
                                auto it = BlockVarValueMaps.find(Pred);
                                if (it != BlockVarValueMaps.end()) {
                                    // Iterate through the map entries of the predecessor
                                    for (const auto &entry : it->second) {
                                        // Check if the key already exists in the union map
                                        auto it_union = unionMap.find(entry.first);
                                        if (it_union == unionMap.end()) {
                                            // Key doesn't exist in union map, add it
                                            unionMap[entry.first] = entry.second;
                                        } else {
                                            // Key exists in union map, compare values
                                            if(entry.second =="BOTTOM" || it_union->second=="BOTTOM"){
                                                it_union->second = "BOTTOM";
                                            }
                                            else if (entry.second != it_union->second) {
                                                // If values are different, mark the key as BOTTOM
                                                it_union->second = "BOTTOM";
                                                
                                            }
                                            else if (entry.second == it_union->second){
                                                continue;
                                            }
                                            else if (entry.second == "TOP" || it_union->second=="TOP"){
                                                it_union->second = "TOP";
                                            }
                                            
                                            
                                        }
                                        
                                    
                                    }
                                }
                            }
                        current_map=unionMap;
                        new_map=processBasicBlock_write_results(CurrBlock, F, current_map);
                        BlockVarValueMaps[CurrBlock] = new_map;
                
                         


                        } else {
                             if(F.getName().str()=="main"){
                            outs()<<"Processing main entry block has single  predecessaors"<<"\n";
                            }
                               new_map = processBasicBlock_write_results(CurrBlock, F, current_map);
                            BlockVarValueMaps[CurrBlock] = new_map;
                        

                        }
                        }
                        

                        if (!compare_maps(new_map, current_map)) {
                            // If there is a change, enqueue successors
                            current_map=new_map;
                            for (auto *Succ : successors(CurrBlock)) {
                                worklist.push(Succ);
                            }
                        }
                    


                    }while (!worklist.empty());
                    // return new_map;
    }