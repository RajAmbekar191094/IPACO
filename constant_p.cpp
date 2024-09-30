#include "llvm/Pass.h"
#include "llvm/IR/Module.h" 
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instructions.h" 
#include "llvm/IR/LegacyPassManager.h"
#include <vector>
#include <fstream>
#include "llvm/IR/Constants.h"
#include "llvm/Analysis/ConstantFolding.h"
#include "unordered_map"
#include <queue>
#include <unordered_set>
#include <stdexcept>
#include <sstream>



using namespace llvm;


namespace {
//  template<typename T>
//  using VarValueMap = std::unordered_map<T, T>;


  std::vector<std::string> results;
  
   void exportResultsToFile(const std::string& filename) {
    std::ofstream outputFile(filename);
    if (outputFile.is_open()) {
      for (const auto& line : results) {
        outputFile << line << "\n";
      }
      outputFile.close();
      errs() << "Results exported to " << filename << "\n";
    } else {
      errs() << "Unable to open file: " << filename << "\n";
    }
  }
struct constant_p : public FunctionPass {
  static char ID;
  int inside_if=0;
       std::unordered_map<BasicBlock*, std::unordered_map<std::string, std::string>> BlockVarValueMaps;
  constant_p() : FunctionPass(ID) {}
    inline std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v")
  {
      s.erase(0, s.find_first_not_of(t));
      return s;
  }
    std::string evaluateBinaryOperation(const std::string& operand1, const std::string& operand2, const std::string& operation) {
        if (operand1 == "BOTTOM" || operand2 == "BOTTOM") {
            return "BOTTOM"; // If any operand is BOTTOM, result is BOTTOM
        } else if (operand1 == "TOP" || operand2 == "TOP") {
            return "TOP"; // If any operand is TOP, result is TOP
        }
      if (operation == "add") {
          // outs()<<operand1<<"\n"<<operand2<<"\n";
          // outs()<<"\n";
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
      } else if (operation == "icmp") {
          // Handle integer comparisons (e.g., eq, ne, sgt, slt, etc.)
          // Implement comparison logic and return result as "0" or "1"
           int op1_int = safe_stoi(operand1);
           int op2_int = safe_stoi(operand2);
           int result = (op1_int > op2_int)? 1:0;
           return std::to_string(result);
      } 
      else {
         
          errs()<<("unsupported op");
           return NULL;
      }
      return NULL;
  }
  int safe_stoi(const std::string& str) {
    std::istringstream iss(str);
    int result;
    if (!(iss >> result)) {
        // Handle invalid input
        return 0; // Or any other appropriate value
    }
    return result;
}
std::string take_meet_load_store(const std::vector<std::string>& predValues) {

    std::string commonValue = predValues[0];
    bool hasTOP = false;
    bool hasNonSame = false;
    bool hasBottom = false;
    
    // Iterate through all values starting from the second one
    for (size_t i = 1; i < predValues.size(); ++i) {
        outs()<<predValues.size()<<"\n";
        // outs()<<"Calculating meet"<<"\n";
        // If any value is different from the commonValue, return "BOTTOM"
        outs()<<predValues[i]<<"\n";
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

  std::string get_predecessor_values_and_meet(BasicBlock *BB, Function &F,std::string value) {
    bool found = false;
                    // Check if the current block has multiple predecessors
                    // if (BB->getSinglePredecessor() == nullptr) {
                        std::vector<std::string> predValues;
                        // Iterate through all predecessors
                        for (auto *Pred : predecessors(BB)) {
                            // Access the map of the predecessor block
                            auto it = BlockVarValueMaps.find(Pred);
                            // outs()<<"RAAAAAAJ"<<"\n";
                            if (it != BlockVarValueMaps.end()) {
                                // Check if the value operand exists in the predecessor's map
                                if (it->second.find(value) != it->second.end()) {
                                    // Update VarValueMap with the value from the predecessor's map
                                    predValues.push_back(it->second[value]);
                                    found = true;
                                    // outs()<<"RAAAAAAJ"<<"\n";
                                }
                            }
                        }
                        for (const auto& value : predValues) {
                                    // errs()<< value << "\n";
                            }
                        if (found) {
                            // If the value operand is found in predecessor maps
                            // Pass all the values to a take_meet function along with BlockVarValueMaps
                            std::string meet_result = take_meet_load_store(predValues);
                            return meet_result;
                            // outs()<<meet_result<<"\n";
                           
                            // VarValueMap[Inst.getName().str()]= meet_result;
                        }
                        else{
                             return "TOP";
                        }
                        

    }
    

std::unordered_map<std::string, std::string> update_map(BasicBlock *BB, Function &F,std::unordered_map<std::string, std::string>& VarValueMap) {
    std::unordered_map<std::string,std::string>local_map;
    // static int block_count=0;
    // block_count++;

    // outs()<<std::to_string(block_count)<<"\n";
      for (auto &Inst : *BB) {
        // if (isa<AllocaInst>(Inst)) {

        //   AllocaInst *Alloca = cast<AllocaInst>(&Inst);             
        //   VarValueMap[Alloca->getName().str()]="TOP";
       
        //   } 
          
        //   errs() << "Variable " << AllocaInst->getName() << " is marked as bottom (non-constant)\n";
        
        if (isa<StoreInst>(&Inst)) {
                StoreInst *Store = cast<StoreInst>(&Inst);
                Value *ValueOperand = Store->getValueOperand();
                Value *PointerOperand = Store->getPointerOperand();

                if (ConstantInt *CI = dyn_cast<ConstantInt>(ValueOperand)) {
                    // If the value operand is a constant integer
                    VarValueMap[PointerOperand->getName().str()] = std::to_string(CI->getSExtValue());
                    local_map[PointerOperand->getName().str()] = std::to_string(CI->getSExtValue());
                } else {
                    // If the value operand is not a constant integer but a variable
                    if (local_map.find(ValueOperand->getName().str()) != local_map.end() &&
                        local_map[ValueOperand->getName().str()] != "TOP" ) {
                        // If the value operand is already present in the current VarValueMap and its value is not TOP or BOTTOM
                        VarValueMap[PointerOperand->getName().str()] = local_map[ValueOperand->getName().str()];
                    } 
                    else if (local_map[ValueOperand->getName().str()] == "BOTTOM"){
                        VarValueMap[PointerOperand->getName().str()]="BOTTOM";
                    }
                    else {
                        bool found = false;
                        // Check if the current block has multiple predecessors
                        // if (BB->getSinglePredecessor() == nullptr) {
                            std::vector<std::string> predValues;
                            // Iterate through all predecessors
                            for (auto *Pred : predecessors(BB)) {
                                // Access the map of the predecessor block
                                auto it = BlockVarValueMaps.find(Pred);
                                if (it != BlockVarValueMaps.end()) {
                                    // Check if the value operand exists in the predecessor's map
                                    if (it->second.find(ValueOperand->getName().str()) != it->second.end()) {
                                        // Update VarValueMap with the value from the predecessor's map
                                        predValues.push_back(it->second[ValueOperand->getName().str()]);
                                        found = true;
                                    }
                                }
                            }
                            if (found) {
                                // If the value operand is found in predecessor maps
                                // Pass all the values to a take_meet function along with BlockVarValueMaps
                                std::string meet_result = take_meet_load_store(predValues);
                                VarValueMap[PointerOperand->getName().str()] = meet_result;
                            } else {
                                // If the variable is not found in the predecessor blocks' maps
                                // Store the variable name without its value
                                VarValueMap[PointerOperand->getName().str()] = VarValueMap[ValueOperand->getName().str()];
                            }
                        // } else {
                        //     // If the current block has only one predecessor, proceed as usual
                        //     VarValueMap[PointerOperand->getName().str()] = VarValueMap[ValueOperand->getName().str()];
                        // }
                    }
                }
            }


        else if (BinaryOperator *BinOp = dyn_cast<BinaryOperator>(&Inst)) {
        Value *Op1 = BinOp->getOperand(0);
        Value *Op2 = BinOp->getOperand(1);
        std::string operation= llvm::Instruction::getOpcodeName(Inst.getOpcode());

        std::string result;

        if (ConstantInt *CI1 = dyn_cast<ConstantInt>(Op1)) {
            // If the first operand is a constant integer
            if (ConstantInt *CI2 = dyn_cast<ConstantInt>(Op2)) {
                // If both operands are constant integers
                // Perform the binary operation and store the result
                std::string val1 = std::to_string(CI1->getSExtValue());
                std::string val2 = std::to_string(CI2->getSExtValue());
                std::string res;
                
                res= evaluateBinaryOperation(val1,val2,operation);
                // result = std::to_string(res);
            } else {
                // If the second operand is a variable
                if (local_map.find(Op2->getName().str()) != local_map.end()) {
                    // If the variable exists in VarValueMap, apply meet operation
                    result = evaluateBinaryOperation(std::to_string(CI1->getSExtValue()), local_map[Op2->getName().str()],operation);
                    VarValueMap[Inst.getName().str()] = result;
                } else {
                    // If the variable does not exist in local_map, it comes from predecessors. take meet
                        //   bool found = false;
                        // // Check if the current block has multiple predecessors
                        // // if (BB->getSinglePredecessor() == nullptr) {
                        //     std::vector<std::string> predValues;
                        //     // Iterate through all predecessors
                        //     for (auto *Pred : predecessors(BB)) {
                        //         // Access the map of the predecessor block
                        //         auto it = BlockVarValueMaps.find(Pred);
                        //         if (it != BlockVarValueMaps.end()) {
                        //             // Check if the value operand exists in the predecessor's map
                        //             if (it->second.find(Op2->getName().str()) != it->second.end()) {
                        //                 // Update VarValueMap with the value from the predecessor's map
                        //                 predValues.push_back(it->second[Op2->getName().str()]);
                        //                 found = true;
                        //             }
                        //         }
                        //     }
                        //     if (found) {
                        //         // If the value operand is found in predecessor maps
                        //         // Pass all the values to a take_meet function along with BlockVarValueMaps
                        //         std::string meet_result = take_meet_load_store(predValues);
                        //         VarValueMap[Op2->getName().str()] = meet_result;
                        //         VarValueMap[Inst.getName().str()]= meet_result;
                        //     }
                    std::string op2_result=get_predecessor_values_and_meet(BB,F,Op2->getName().str());
                
                    VarValueMap[Op2->getName().str()] = op2_result;
                    local_map[Op2->getName().str()]= op2_result;
                    VarValueMap[Inst.getName().str()] = op2_result;
                //             } else {
                //                 // If the variable is not found in the predecessor blocks' maps
                //                 // Store the variable name without its value i.e TOP
                //                 VarValueMap[Op2->getName().str()] = "TOP";
                //             }
                // }
            }
            }} 
            // If the first operand is a variable and second is constant
            
            
        else if (ConstantInt *CI2 = dyn_cast<ConstantInt>(Op2)) {
                // If the second operand is a constant integer
                // Apply meet operation
                if (local_map.find(Op1->getName().str()) != local_map.end()){
                // If the variable exists in VarValueMap, apply meet operation
                result = evaluateBinaryOperation(local_map[Op1->getName().str()],std::to_string(CI2->getSExtValue()),operation);
                VarValueMap[Inst.getName().str()] = result;
                }
                  std::string op1_result=get_predecessor_values_and_meet(BB,F,Op1->getName().str());
                
                    VarValueMap[Op1->getName().str()] = op1_result;
                    local_map[Op1->getName().str()]= op1_result;
                    VarValueMap[Inst.getName().str()] = op1_result;
                // bool found = false;
                    // Check if the current block has multiple predecessors
            //         if (BB->getSinglePredecessor() == nullptr) {
            //             std::vector<std::string> predValues;
            //             // Iterate through all predecessors
            //             for (auto *Pred : predecessors(BB)) {
            //                 // Access the map of the predecessor block
            //                 auto it = BlockVarValueMaps.find(Pred);
            //                 if (it != BlockVarValueMaps.end()) {
            //                     // Check if the value operand exists in the predecessor's map
            //                     if (it->second.find(Op1->getName().str()) != it->second.end()) {
            //                         // Update VarValueMap with the value from the predecessor's map
            //                         predValues.push_back(it->second[Op1->getName().str()]);
            //                         found = true;
            //                     }
            //                 }
            //             }
            //             if (found) {
            //                 // If the value operand is found in predecessor maps
            //                 // Pass all the values to a take_meet function along with BlockVarValueMaps
            //                 std::string meet_result = take_meet_load_store(predValues);
            //                 VarValueMap[Op1->getName().str()] = meet_result;
            //                 VarValueMap[Inst.getName().str()]= meet_result;
            //             } else {
            //                 // If the variable is not found in the predecessor blocks' maps
            //                 // Store the variable name without its value i.e TOP
            //                 VarValueMap[Op1->getName().str()] = "TOP";
            //                 VarValueMap[Inst.getName().str()] = "TOP";
            //             }
            // }
            } else {
                // If both the  operands are a variable
                if (local_map.find(Op1->getName().str()) != local_map.end() && local_map.find(Op2->getName().str()) != local_map.end()){
                    result = evaluateBinaryOperation(local_map[Op1->getName().str()],local_map[Op2->getName().str()],operation);
                    VarValueMap[Inst.getName().str()] = result;

            }
                  if (local_map.find(Op1->getName().str()) == local_map.end() && local_map.find(Op2->getName().str()) != local_map.end()){
                    std::string op1_result=get_predecessor_values_and_meet(BB,F,Op1->getName().str());
                    result = evaluateBinaryOperation(op1_result,local_map[Op2->getName().str()],operation);
                    VarValueMap[Op1->getName().str()]=op1_result;
                    local_map[Op1->getName().str()]=op1_result;
                    VarValueMap[Inst.getName().str()] = result;

            }
                  if (local_map.find(Op1->getName().str()) != local_map.end() && local_map.find(Op2->getName().str()) == local_map.end()){
                    std::string op2_result=get_predecessor_values_and_meet(BB,F,Op2->getName().str());
                    result = evaluateBinaryOperation(local_map[Op1->getName().str()],op2_result,operation);
                    VarValueMap[Op2->getName().str()]=op2_result;
                    local_map[Op2->getName().str()]=op2_result;
                    VarValueMap[Inst.getName().str()] = result;

            }
                  if (local_map.find(Op1->getName().str()) == local_map.end() && local_map.find(Op2->getName().str()) == local_map.end()){
                    bool op1Found = false, op2Found = false;
                    std::vector<std::string> predMaps1;
                    std::vector<std::string> predMaps2;
                    std::vector<std::string> dest_operand;
                    // Check each predecessor map for the presence of operands
                    for (auto *Pred : predecessors(BB)) {
                        auto it = BlockVarValueMaps.find(Pred);
                        if (it != BlockVarValueMaps.end()) {
                            if (it->second.find(Op1->getName().str()) != it->second.end()) {
                                op1Found = true;
                                predMaps1.push_back(Op1->getName().str());
                            }
                            if (it->second.find(Op2->getName().str()) != it->second.end()) {
                                op2Found = true;
                                predMaps2.push_back(Op2->getName().str());
                            }
                            if(op1Found && op2Found){
                            std::string dest_val=evaluateBinaryOperation(Op1->getName().str(),Op2->getName().str(),operation) ;
                            dest_operand.push_back(dest_val);


                            }
                        }
                    }
                      VarValueMap[Op1->getName().str()]=take_meet_load_store(predMaps1);
                      VarValueMap[Op2->getName().str()]=take_meet_load_store(predMaps2);
                      VarValueMap[Inst.getName().str()] = take_meet_load_store(dest_operand);
            }
            } 
    }

            


            else if (LoadInst *Load = dyn_cast<LoadInst>(&Inst)) {
                Value *PtrOperand = Load->getPointerOperand();
                Value *Operand = Load->getOperand(0);
                std::string instructionString;
                raw_string_ostream os(instructionString);
                Inst.print(os); // Print the instruction to a string
                std::string lhs = os.str();
                ltrim(lhs);

                if (local_map.find(PtrOperand->getName().str()) != local_map.end() &&
                    local_map[PtrOperand->getName().str()] != "TOP"){
                    // If the pointer operand is already present in the current VarValueMap and its value is not TOP or BOTTOM
                    VarValueMap[lhs.substr(0, 2)] = local_map[PtrOperand->getName().str()];
                } else {
                    
                    // bool found = false;
                    // // Check if the current block has multiple predecessors
                    // if (BB->getSinglePredecessor() == nullptr) {
                    //     std::vector<std::string> predValues;
                    //     // Iterate through all predecessors
                    //     for (auto *Pred : predecessors(BB)) {
                    //         // Access the map of the predecessor block
                    //         auto it = BlockVarValueMaps.find(Pred);
                    //         if (it != BlockVarValueMaps.end()) {
                    //             // Check if the pointer operand exists in the predecessor's map
                    //             if (it->second.find(PtrOperand->getName().str()) != it->second.end()) {
                    //                 // Update VarValueMap with the value from the predecessor's map
                    //                 predValues.push_back(it->second[PtrOperand->getName().str()]);
                    //                 found = true;
                    //             }
                    //         }
                    //     }
                    //     if (found) {
                    //         // If the pointer operand is found in predecessor maps
                    //         // Pass all the values to a take_meet function along with BlockVarValueMaps
                    //         std::string meet_result = take_meet_load_store(predValues);
                    //         VarValueMap[lhs.substr(0, 2)] = meet_result;
                    //     } else {
                    //         // If the pointer is not found in the predecessor blocks' maps
                    //         // Store the pointer operand name without its value
                    //         VarValueMap[lhs.substr(0, 2)] = PtrOperand->getName().str();
                    //     }
                    // } else {
                    //     // If the current block has only one predecessor, proceed as usual
                    //     VarValueMap[lhs.substr(0, 2)] = PtrOperand->getName().str();
                    // }
                    std::string operand_result=get_predecessor_values_and_meet(BB,F,PtrOperand->getName().str());
                    outs()<<operand_result<<"\n";
                    VarValueMap[lhs.substr(0, 2)] = operand_result;
                    VarValueMap[PtrOperand->getName().str()] = operand_result;
                }
            }


            if(CallInst *Call = dyn_cast<CallInst>(&Inst)){
                if (isa<CallInst>(&Inst)) {
                  Function *CalledFunc = Call->getCalledFunction();
                  if (CalledFunc && CalledFunc->getName() == "__isoc99_scanf") {
                      Value *Arg = Call->getArgOperand(1);
                      if (Arg->hasName()) {
                        
                        VarValueMap[Arg->getName().str()]="BOTTOM";
                        local_map[Arg->getName().str()]="BOTTOM";
                        
                  
                      
                      }
                  }
        

            }
          }
          if (isa<ICmpInst>(&Inst) ) {
            ICmpInst *IC = dyn_cast<ICmpInst>(&Inst);
                Value *Op1 = Inst.getOperand(0);
                Value *Op2 = Inst.getOperand(1);
                std::string operation= "icmp";

        std::string result;

        if (ConstantInt *CI1 = dyn_cast<ConstantInt>(Op1)) {
            // If the first operand is a constant integer
            if (ConstantInt *CI2 = dyn_cast<ConstantInt>(Op2)) {
                // If both operands are constant integers
                // Perform the binary operation and store the result
                std::string val1 = std::to_string(CI1->getSExtValue());
                std::string val2 = std::to_string(CI2->getSExtValue());
                std::string res;
                
                res= evaluateBinaryOperation(val1,val2,operation);
                // result = std::to_string(res);
            } else {
                // If the second operand is a variable
                if (local_map.find(Op2->getName().str()) != local_map.end()) {
                    // If the variable exists in VarValueMap, apply meet operation
                    result = evaluateBinaryOperation(std::to_string(CI1->getSExtValue()), local_map[Op2->getName().str()],operation);
                    VarValueMap[Inst.getName().str()] = result;
                } else {
                    
                    std::string op2_result=get_predecessor_values_and_meet(BB,F,Op2->getName().str());
                
                    VarValueMap[Op2->getName().str()] = op2_result;
                    local_map[Op2->getName().str()]= op2_result;
                    VarValueMap[Inst.getName().str()] = op2_result;
                //             } else {
                //                 // If the variable is not found in the predecessor blocks' maps
                //                 // Store the variable name without its value i.e TOP
                //                 VarValueMap[Op2->getName().str()] = "TOP";
                //             }
                // }
            }
            }} 
            // If the first operand is a variable and second is constant
            
            
        else if (ConstantInt *CI2 = dyn_cast<ConstantInt>(Op2)) {
                // If the second operand is a constant integer
                // Apply meet operation
                if (local_map.find(Op1->getName().str()) != local_map.end()){
                // If the variable exists in VarValueMap, apply meet operation
                result = evaluateBinaryOperation(local_map[Op1->getName().str()],std::to_string(CI2->getSExtValue()),operation);
                VarValueMap[Inst.getName().str()] = result;
                }
                  std::string op1_result=get_predecessor_values_and_meet(BB,F,Op1->getName().str());
                
                    VarValueMap[Op1->getName().str()] = op1_result;
                    local_map[Op1->getName().str()]= op1_result;
                    VarValueMap[Inst.getName().str()] = op1_result;
               
            } else {
                // If both the  operands are a variable
                if (local_map.find(Op1->getName().str()) != local_map.end() && local_map.find(Op2->getName().str()) != local_map.end()){
                    result = evaluateBinaryOperation(local_map[Op1->getName().str()],local_map[Op2->getName().str()],operation);
                    VarValueMap[Inst.getName().str()] = result;

            }
                  if (local_map.find(Op1->getName().str()) == local_map.end() && local_map.find(Op2->getName().str()) != local_map.end()){
                    std::string op1_result=get_predecessor_values_and_meet(BB,F,Op1->getName().str());
                    result = evaluateBinaryOperation(op1_result,local_map[Op2->getName().str()],operation);
                    VarValueMap[Op1->getName().str()]=op1_result;
                    local_map[Op1->getName().str()]=op1_result;
                    VarValueMap[Inst.getName().str()] = result;

            }
                  if (local_map.find(Op1->getName().str()) != local_map.end() && local_map.find(Op2->getName().str()) == local_map.end()){
                    std::string op2_result=get_predecessor_values_and_meet(BB,F,Op2->getName().str());
                    result = evaluateBinaryOperation(local_map[Op1->getName().str()],op2_result,operation);
                    VarValueMap[Op2->getName().str()]=op2_result;
                    local_map[Op2->getName().str()]=op2_result;
                    VarValueMap[Inst.getName().str()] = result;

            }
                  if (local_map.find(Op1->getName().str()) == local_map.end() && local_map.find(Op2->getName().str()) == local_map.end()){
                    bool op1Found = false, op2Found = false;
                    std::vector<std::string> predMaps1;
                    std::vector<std::string> predMaps2;
                    std::vector<std::string> dest_operand;
                    // Check each predecessor map for the presence of operands
                    for (auto *Pred : predecessors(BB)) {
                        auto it = BlockVarValueMaps.find(Pred);
                        if (it != BlockVarValueMaps.end()) {
                            if (it->second.find(Op1->getName().str()) != it->second.end()) {
                                op1Found = true;
                                predMaps1.push_back(Op1->getName().str());
                            }
                            if (it->second.find(Op2->getName().str()) != it->second.end()) {
                                op2Found = true;
                                predMaps2.push_back(Op2->getName().str());
                            }
                            if(op1Found && op2Found){
                            std::string dest_val=evaluateBinaryOperation(Op1->getName().str(),Op2->getName().str(),operation) ;
                            dest_operand.push_back(dest_val);


                            }
                        }
                    }
                      VarValueMap[Op1->getName().str()]=take_meet_load_store(predMaps1);
                      VarValueMap[Op2->getName().str()]=take_meet_load_store(predMaps2);
                      VarValueMap[Inst.getName().str()] = take_meet_load_store(dest_operand);
            }
            } 
                    
                }
                // Check if the instruction is a branch instruction (br)
          }
 

 
  // }
  return VarValueMap;
  }



std::unordered_map<std::string, std::string>  processBasicBlock_write_results(BasicBlock *BB, Function &F,std::unordered_map<std::string, std::string>& VarValueMap,bool multi_pred) {
    // std::unordered_map<std::string,std::string>VarValueMap;
    static int block_count=0;
    block_count++;

    // outs()<<std::to_string(block_count)<<"\n";
      for (auto &Inst : *BB) {
        if (isa<AllocaInst>(Inst)) {

          std::string instructionString;
          raw_string_ostream os(instructionString);
          Inst.print(os); // Print the instruction to a string
          
          AllocaInst *Alloca = cast<AllocaInst>(&Inst);
          std::string lhs= os.str();
          lhs += "  --> %" + Alloca->getName().str();
          ltrim(lhs);
          lhs += "=TOP";     
          VarValueMap[Alloca->getName().str()]="TOP";  
          results.push_back(lhs);
          } 
          
        //   errs() << "Variable " << AllocaInst->getName() << " is marked as bottom (non-constant)\n";
        
        else if(isa<StoreInst>(&Inst)) {
          // If it's a StoreInst, check if the value being stored is a constant.
   
            // The value is a constant, mark the variable as constant.
            // You can store this information in a data structure or use metadata.
            // For simplicity, let's just print the information here.
         
          StoreInst *Store = cast<StoreInst>(&Inst);
          Value *ValueOperand = Store->getValueOperand();
          Value *PointerOperand = Store->getPointerOperand();
          std::string instructionString;
          raw_string_ostream os(instructionString);
          Inst.print(os); // Print the instruction to a string
          std::string lhs= os.str();
          lhs +=  "  --> %" + PointerOperand->getName().str();
          ltrim(lhs);
           
         if (ConstantInt *CI = dyn_cast<ConstantInt>(ValueOperand)) {
          
            // ConstantInt *CI = dyn_cast<ConstantInt>(ValueOperand);
        
            // std::string rhs = std::to_string(CI->getSExtValue());
            std::string rhs = "="+ std::to_string(CI->getSExtValue());
            VarValueMap[PointerOperand->getName().str()]=std::to_string(CI->getSExtValue());
          
            results.push_back(lhs+rhs);
            // outs()<<(lhs+rhs);
         }
        
          else {
          // If the value operand is not a constant integer
          // You can handle non-constant value operands as per your requirements
          // For simplicity, let's just store the variable name without its value
          // lhs += "  --> %" + PointerOperand->getName().str();
          // ltrim(lhs);
          VarValueMap[PointerOperand->getName().str()]=VarValueMap[ValueOperand->getName().str()];
          std::string rhs = " = " + VarValueMap[PointerOperand->getName().str()]+ "," + ValueOperand->getName().str() +"="+VarValueMap[PointerOperand->getName().str()];
          results.push_back(lhs + rhs);
          }
        }
        else if (isa<BinaryOperator>(&Inst)) {
            // for(const auto& elem : VarValueMap)
            // {
            //   outs()<< elem.first << " " << elem.second << "\n";
             
            // }
            // outs()<<"**********"<<"\n";
           BinaryOperator *Bin = cast<BinaryOperator>(&Inst);
            Value *Op1 = Inst.getOperand(0);
            Value *Op2 = Inst.getOperand(1);
            bool op1const=0,op2const=0;

            unsigned NumOperands = Bin->getNumOperands();

              // Iterate through all operands
            std::string op1String;
            raw_string_ostream os1(op1String);
            Op1->print(os1);
            
            std::string op1 = os1.str();
            ltrim(op1);
            // outs()<<op1.substr(0,2)<<"\n";
           

           std::string op2String;
            raw_string_ostream os2(op1String);
            Op2->print(os2);
           
            std::string op2 = os2.str();
            ltrim(op2);
            // outs()<<op2.substr(0,2)<<"\n";
            
  
            std::string operation= llvm::Instruction::getOpcodeName(Inst.getOpcode());
  

            std::string  op1Value,op2Value;

            // Check if Op1 is a constant integer or a variable
            if (ConstantInt *CI1 = dyn_cast<ConstantInt>(Op1)) {
                op1const=1;
                op1Value = std::to_string(CI1->getSExtValue()); // Operand 1 is a constant integer
                // outs()<<"Mistake :"+op1Value;
            } else {
                // Operand 1 is a variable, check if its value is in VarValueMap
                 op1const=0;
                if (VarValueMap.find(op1.substr(0,2)) != VarValueMap.end()) {
                    // for(const auto& elem : VarValueMap)
                    // {
                    //   outs()<< elem.first << " " << elem.second << "\n";
                    
                    // }
         
                    // errs() << "Binary operator: " << *Bin << "\n";
                    // errs() << "Operand 1: " << (*Op1).substr(0,2)<<"\n";
                    // outs()<<op1<<"\n";
                    // outs()<<op1.substr(0,2)<<"\n";
                    op1Value = VarValueMap[op1.substr(0,2)]; // Get the value from VarValueMap
                    // outs()<<"Mistake :"+Op1->getName().str()+":"+VarValueMap[Op1->getName().str()];
                } else {
                    // Handle error if operand value not found in VarValueMap
                    //  outs()<<"RAAj Mistake :"+op1Value;
                }
            }

            // Check if Op2 is a constant integer or a variable
            if (ConstantInt *CI2 = dyn_cast<ConstantInt>(Op2)) {
                op2const=1;
                op2Value = std::to_string(CI2->getSExtValue()); // Operand 2 is a constant integer
            } else {
              op2const=0;
                // Operand 2 is a variable, check if its value is in VarValueMap
                if (VarValueMap.find(op2.substr(0,2)) != VarValueMap.end()) {
                   
                    op2Value = VarValueMap[op2.substr(0,2)]; // Get the value from VarValueMap
                    // outs()<<"Mistake :"+Op2->getName().str()+":"+VarValueMap[Op2->getName().str()];
                } else {
                    // Handle error if operand value not found in VarValueMap
                }
            }

            // Perform binary operation
            // Evaluate the result using op1Value and op2Value
            // Handle binary operation (addition, subtraction, etc.) based on the instruction's opcode
            // Store the result in VarValueMap if needed

            // Example:
          //  outs()<<op1Value<<"\t"<<op2Value<<"\n";
          std::string instructionString;
            raw_string_ostream os(instructionString);
            Inst.print(os); // Print the instruction to a string
            std::string lhs = os.str();
            lhs += "  --> %" + Inst.getName().str();
            ltrim(lhs);

          std::string rhs;
          if(!multi_pred){
             std::string result = evaluateBinaryOperation(op1Value, op2Value,operation);
             if(op1const && op2const) {rhs = result;} 
             else if(op1const && !op2const){rhs = result+", "+op2.substr(0,2)+"="+VarValueMap[op2.substr(0,2)]; }
             else if(!op1const && op2const){rhs = result+", "+op1.substr(0,2)+"="+VarValueMap[op1.substr(0,2)]; }
             else{ rhs = result+", "+op1.substr(0,2)+"="+VarValueMap[op1.substr(0,2)]+" ,"+op2.substr(0,2)+"="+VarValueMap[op2.substr(0,2)];}

                

            VarValueMap[Inst.getName().str()] = result;
            results.push_back(lhs + "=" + rhs);
           
          }

          else{
            
            if(op1const && op2const) {rhs = VarValueMap[Inst.getName().str()];} 
             else if(op1const && !op2const){rhs = VarValueMap[Inst.getName().str()]+", "+op2.substr(0,2)+"="+VarValueMap[op2.substr(0,2)]; }
             else if(!op1const && op2const){rhs = VarValueMap[Inst.getName().str()]+", "+op1.substr(0,2)+"="+VarValueMap[op1.substr(0,2)]; }
             else{ rhs = VarValueMap[Inst.getName().str()]+", "+op1.substr(0,2)+"="+VarValueMap[op1.substr(0,2)]+" ,"+op2.substr(0,2)+"="+VarValueMap[op2.substr(0,2)];}
            results.push_back(lhs + "=" + rhs);

          }
           
            // VarValueMap[Inst.getName().str()] = std::to_string(result);

            // Construct the result string
            

        }

       else if (LoadInst *Load = dyn_cast<LoadInst>(&Inst)) {
        // outs()<<"RAAAAAAJ"<<"\n";
            Value *PtrOperand = Load->getPointerOperand();
            Value *Operand = Load->getOperand(0);
            std::string instructionString;
            raw_string_ostream os(instructionString);
            Inst.print(os); // Print the instrucTOPtion to a string
            std::string lhs= os.str();
            std::string destVarName = Load->getName().str();

                    // Get the constant value loaded by the load instruction
              Value* loadedValue = Load->getOperand(0);
              std::string pointerName = Load->getPointerOperand()->getName().str();
            // if (auto* constant = dyn_cast<ConstantInt>(loadedValue)) {
              
                lhs += "  --> ";
            // lhs.erase(std::remove_if(lhs.begin(), lhs.end(), ::isspace),lhs.end());
               
                ltrim(lhs);
              // int pointerValue = 0;
              // if (auto* constantPtr = dyn_cast<ConstantInt>(Load->getPointerOperand())) {
              //     pointerValue = constantPtr->getSExtValue();
              // }
                // outs()<<lhs<<"\n";
                // outs()<<lhs.substr(0, 5)<<"\n";
                std::string rhs= lhs.substr(0,2)+"="+VarValueMap[pointerName]+ ", %" +pointerName+"="+VarValueMap[pointerName];
                // outs()<<VarValueMap[pointerName]<<"\n";
                // outs()<<pointerName<<"\n";
                // outs()<<rhs<<"\n";

                VarValueMap[lhs.substr(0,2)]=VarValueMap[Load->getPointerOperand()->getName().str()];
                results.push_back(lhs+rhs);

       }
        if(CallInst *Call = dyn_cast<CallInst>(&Inst)){
            if (isa<CallInst>(&Inst)) {
              Function *CalledFunc = Call->getCalledFunction();
              if (CalledFunc && CalledFunc->getName() == "__isoc99_scanf") {
                  Value *Arg = Call->getArgOperand(1);
                  if (Arg->hasName()) {
                    std::string instructionString;
                    raw_string_ostream os(instructionString);
                    Inst.print(os); // Print the instruction to a string
                    std::string lhs= os.str();
                    lhs += "  --> %" + Arg->getName().str();
                    ltrim(lhs);
                    VarValueMap[Arg->getName().str()]="BOTTOM";
                   
                    results.push_back(lhs + "=BOTTOM");
                   
                  }
              }
    

        }
      }
      if (isa<ICmpInst>(&Inst) || isa<FCmpInst>(&Inst)) {
                std::string instructionString;
                raw_string_ostream os(instructionString);
                Inst.print(os); // Print the instruction to a string
                std::string comp_inst= os.str();
                 ltrim(comp_inst);
                Value *op1 = Inst.getOperand(0);
                Value *op2 = Inst.getOperand(1);

                  // Variables to hold operand values
                std::string op1_value, op2_value;

                // Check if Operand 1 is a constant or needs to be fetched from VarValueMap
                if (auto *op1_const = dyn_cast<ConstantInt>(op1)) {
                    op1_value = std::to_string(op1_const->getSExtValue());
                } else {
                    op1_value = VarValueMap[op1->getName().str()];
                }

                // Check if Operand 2 is a constant or needs to be fetched from VarValueMap
                if (auto *op2_const = dyn_cast<ConstantInt>(op2)) {
                    op2_value = std::to_string(op2_const->getSExtValue());
                } else {
                    op2_value = VarValueMap[op2->getName().str()];
                }

                // Perform the comparison and declare cmp accordingly
                if (op1_value == "TOP" || op2_value == "TOP") {
                    results.push_back(comp_inst += "TOP");
                }
                else if (op1_value == "BOTTOM" || op2_value == "BOTTOM"){

                       results.push_back(comp_inst += "BOTTOM");
                }
                else {
                    int op1_int = safe_stoi(op1_value);
                    int op2_int = safe_stoi(op2_value);
                    int result = (op1_int > op2_int)? 1:0;
                    results.push_back(comp_inst += " %cmp = "+std::to_string(result));
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
  bool runOnFunction(Function &F) override {
   
  
    std::queue<BasicBlock *> BBQueue;
    std::unordered_set<BasicBlock *> visited;
    // std::unordered_map<std::string, std::string> current_VarValueMap;
    std::unordered_map<std::string, std::string> next_VarValueMap;

    bool multi_pred=0;
    // Process the entry block
    BasicBlock *EntryBlock = &F.getEntryBlock();
    next_VarValueMap = processBasicBlock_write_results(EntryBlock, F,next_VarValueMap,multi_pred=0);
    BlockVarValueMaps[EntryBlock] = next_VarValueMap;

    visited.insert(EntryBlock);

    // Enqueue the entry block's successors
    for (auto *Succ : successors(EntryBlock)) {

        BBQueue.push(Succ);
        visited.insert(Succ);
    }

    // Process each basic block in BFS order
    while (!BBQueue.empty()) {
        BasicBlock *CurrBlock = BBQueue.front();
        BBQueue.pop();
 
        // Process the current basic block and get the next VarValueMap
        if (CurrBlock->getSinglePredecessor() != nullptr) {
          // If the block has a single predecessor, write results and update map
          next_VarValueMap = processBasicBlock_write_results(CurrBlock, F, next_VarValueMap,multi_pred=0);
          BlockVarValueMaps[CurrBlock] = next_VarValueMap;
        } else {
          // If the block has multiple predecessors, take union of maps and then update with end block
          std::unordered_map<std::string, std::string> unionMap;
    bool has_bottom = false;
    bool has_top = false;
    
    // Iterate through predecessors
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

          BlockVarValueMaps[CurrBlock] = update_map(CurrBlock, F, unionMap);
        //   for (auto &entry : BlockVarValueMaps[CurrBlock]) {
        //         outs() << entry.first << " : " << entry.second << "\n";
        //     }
          BlockVarValueMaps[CurrBlock] = processBasicBlock_write_results(CurrBlock, F, BlockVarValueMaps[CurrBlock],multi_pred=1);
        }

        // Enqueue unvisited successor blocks
        for (auto *Succ : successors(CurrBlock)) {
            if (visited.find(Succ) == visited.end()) {
                BBQueue.push(Succ);
                visited.insert(Succ);
            }
        }

        // Update next_VarValueMap for the next iteration
        // next_VarValueMap = blockVarValueMap;
    }

    // Visit all remaining basic blocks (unreachable blocks)
    for (auto &BB : F) {
        if (visited.find(&BB) == visited.end()) {
            next_VarValueMap=processBasicBlock_write_results(&BB, F, next_VarValueMap,multi_pred=0);
            visited.insert(&BB);
        }
    }



     
    
  
      exportResultsToFile("output.txt");
      // for(auto it = VarValueMap.cbegin(); it != VarValueMap.cend(); ++it)
      // {
      //     outs() << it->first << " " << it->second << "\n";
      // }


    return false;
}
}; // end of struct constant_p

std::ofstream outputFile("output.txt");
// Function to evaluate binary operations

// Check if the file is opened successfully
// if (outputFile.is_open()) {
//     // Iterate over the vector and write each element to the file
//     for (const auto& line : results) {
//         outputFile << line << std::endl;
//     }
    
//     // Close the file after writing
//     outputFile.close();
//     std::cout << "Data has been written to output.txt" << std::endl;
// } else {
//     std::cerr << "Unable to open the output file" << std::endl;
// }
// trim from left

}  // end of anonymous namespace

char constant_p::ID = 0;
static RegisterPass<constant_p> X("constant_p", "Constant Propagation Pass for Assignment",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
