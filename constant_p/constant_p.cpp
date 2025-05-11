// //output.txt will be generated on the same path where the opt pass is run

// //Assumptions:
// // Only integer values considered
// // Only binary operators considered

// #include "llvm/Pass.h"
// #include "llvm/IR/Module.h" 
// #include "llvm/IR/Function.h"
// #include "llvm/Support/raw_ostream.h"
// #include "llvm/IR/Instructions.h" 
// #include "llvm/IR/LegacyPassManager.h"
// #include <vector>
// #include <fstream>
// #include "llvm/IR/Constants.h"
// #include "unordered_map"
// #include <queue>
// #include <unordered_set>
// #include <stdexcept>
// #include <sstream>



// using namespace llvm;


// namespace {


// bool compare_maps(const std::unordered_map<std::string, std::string>& map1, const std::unordered_map<std::string, std::string>& map2) {
//     // If the sizes of the maps are different, they cannot be equal
//     if (map1.size() != map2.size()) {
//         return false;
//     }

//     // Iterate over elements of map1
//     for (const auto& pair : map1) {
//         // Check if the key exists in map2 and if its corresponding value is the same
//         auto it = map2.find(pair.first);
//         if (it == map2.end() || it->second != pair.second) {
//             return false;
//         }
//     }

//     // Iterate over elements of map2 to ensure all keys are present in map1 (already checked) and have same values
//     for (const auto& pair : map2) {
//         auto it = map1.find(pair.first);
//         if (it == map1.end() || it->second != pair.second) {
//             return false;
//         }
//     }

//     // Maps are equal
//     return true;
// }


//   std::vector<std::string> results;
  
//    void exportResultsToFile(const std::string& filename) {
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
// struct constant_p : public FunctionPass {
//   static char ID;
//   int inside_if=0;
//        std::unordered_map<BasicBlock*, std::unordered_map<std::string, std::string>> BlockVarValueMaps;
//   constant_p() : FunctionPass(ID) {}
//     inline std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v")
//   {
//       s.erase(0, s.find_first_not_of(t));
//       return s;
//   }
//     std::string evaluateBinaryOperation(const std::string& operand1, const std::string& operand2, const std::string& operation) {
//         if (operand1 == "BOTTOM" || operand2 == "BOTTOM") {
//             return "BOTTOM"; // If any operand is BOTTOM, result is BOTTOM
//         } else if (operand1 == "TOP" || operand2 == "TOP") {
//             return "TOP"; // If any operand is TOP, result is TOP
//         }
//       if (operation == "add") {
         
//           return std::to_string(safe_stoi(operand1) + safe_stoi(operand2));
//       } else if (operation == "sub") {
//           return std::to_string(safe_stoi(operand1) - safe_stoi(operand2));
//       } else if (operation == "mul") {
//           return std::to_string(safe_stoi(operand1) * safe_stoi(operand2));
//       } else if (operation == "sdiv") {
//           if (std::stoi(operand2) == 0) {
//             outs()<<("Division by zero error");
//           }
//           return std::to_string(safe_stoi(operand1) / safe_stoi(operand2));
//       } else if (operation == "srem") {
//           if (std::stoi(operand2) == 0) {
//               outs()<<("Division by zero error");
//           }
//           return std::to_string(safe_stoi(operand1) % safe_stoi(operand2));
//       } else if (operation == "icmp_eq") {
//           // Handle integer comparisons (e.g., eq, ne, sgt, slt, etc.)
//           // Implement comparison logic and return result as "0" or "1"
//            int op1_int = safe_stoi(operand1);
//            int op2_int = safe_stoi(operand2);
//            int result = (op1_int == op2_int)? 1:0;
//            return std::to_string(result);
//       } 
//       else if (operation == "icmp_ne") {
//           // Handle integer comparisons (e.g., eq, ne, sgt, slt, etc.)
//           // Implement comparison logic and return result as "0" or "1"
//            int op1_int = safe_stoi(operand1);
//            int op2_int = safe_stoi(operand2);
//            int result = (op1_int != op2_int)? 1:0;
//            return std::to_string(result);
//       } 
//       else if (operation == "icmp_sgt") {
//           // Handle integer comparisons (e.g., eq, ne, sgt, slt, etc.)
//           // Implement comparison logic and return result as "0" or "1"
//            int op1_int = safe_stoi(operand1);
//            int op2_int = safe_stoi(operand2);
//            int result = (op1_int > op2_int)? 1:0;
//            return std::to_string(result);
//       } 
//       else if (operation == "icmp_slt") {
//           // Handle integer comparisons (e.g., eq, ne, sgt, slt, etc.)
//           // Implement comparison logic and return result as "0" or "1"
//            int op1_int = safe_stoi(operand1);
//            int op2_int = safe_stoi(operand2);
//            int result = (op1_int < op2_int)? 1:0;
//            return std::to_string(result);
//       } 
//        else if (operation == "icmp_sge") {
//           // Handle integer comparisons (e.g., eq, ne, sgt, slt, etc.)
//           // Implement comparison logic and return result as "0" or "1"
//            int op1_int = safe_stoi(operand1);
//            int op2_int = safe_stoi(operand2);
//            int result = (op1_int >= op2_int)? 1:0;
//            return std::to_string(result);
//       } 
//       else if (operation == "icmp_sle") {
//           // Handle integer comparisons (e.g., eq, ne, sgt, slt, etc.)
//           // Implement comparison logic and return result as "0" or "1"
//            int op1_int = safe_stoi(operand1);
//            int op2_int = safe_stoi(operand2);
//            int result = (op1_int <= op2_int)? 1:0;
//            return std::to_string(result);
//       } 
//       else if (operation == "icmp_ugt") {
//           // Handle integer comparisons (e.g., eq, ne, sgt, slt, etc.)
//           // Implement comparison logic and return result as "0" or "1"
//            int op1_int = safe_stoi(operand1);
//            int op2_int = safe_stoi(operand2);
//            int result = (op1_int > op2_int)? 1:0;
//            return std::to_string(result);
//       } 
//       else if (operation == "icmp_ult") {
//           // Handle integer comparisons (e.g., eq, ne, sgt, slt, etc.)
//           // Implement comparison logic and return result as "0" or "1"
//            int op1_int = safe_stoi(operand1);
//            int op2_int = safe_stoi(operand2);
//            int result = (op1_int < op2_int)? 1:0;
//            return std::to_string(result);
//       } else if (operation == "icmp_uge") {
//           // Handle integer comparisons (e.g., eq, ne, sgt, slt, etc.)
//           // Implement comparison logic and return result as "0" or "1"
//            int op1_int = safe_stoi(operand1);
//            int op2_int = safe_stoi(operand2);
//            int result = (op1_int >= op2_int)? 1:0;
//            return std::to_string(result);
//       } 
//       else if (operation == "icmp_ule") {
//           // Handle integer comparisons (e.g., eq, ne, sgt, slt, etc.)
//           // Implement comparison logic and return result as "0" or "1"
//            int op1_int = safe_stoi(operand1);
//            int op2_int = safe_stoi(operand2);
//            int result = (op1_int <= op2_int)? 1:0;
//            return std::to_string(result);
//       } 
//       else {
         
//           errs()<<("unsupported op");
//            return NULL;
//       }
//       return NULL;
//   }
//   int safe_stoi(const std::string& str) {
//     std::istringstream iss(str);
//     int result;
//     if (!(iss >> result)) {
//         // Handle invalid input
//         return 0; // Or any other appropriate value
//     }
//     return result;
// }
// std::string meet_operation(const std::vector<std::string>& predValues) {

//     std::string commonValue = predValues[0];
//     bool hasTOP = false;
//     bool hasNonSame = false;
    
//     // Iterate through all values starting from the second one
//     for (size_t i = 1; i < predValues.size(); ++i) {
       
//         if (predValues[i] == "TOP") {
//             hasTOP = true;
          
//         } else if (predValues[i] == "BOTTOM") {
//             return "BOTTOM";
//         } else {
//             if (predValues[i] != commonValue) {
//                 hasNonSame = true;
//                 break;
//             }
            
//         }
//     }
//     if(hasNonSame){
//         // outs()<<"Should return"<<"\n";
//       return "BOTTOM"; 
//     }
//     else if(hasTOP && !hasNonSame){
//       return "TOP";
//     }
//     else{
//         //  outs()<<"Should not return"<<"\n";
//       return commonValue;
//     }
//     }
//     std::string predicateToString(CmpInst::Predicate pred) {
//     switch (pred) {
//         case CmpInst::ICMP_EQ: return "eq";
//         case CmpInst::ICMP_NE: return "ne";
//         case CmpInst::ICMP_UGT: return "ugt";
//         case CmpInst::ICMP_UGE: return "uge";
//         case CmpInst::ICMP_ULT: return "ult";
//         case CmpInst::ICMP_ULE: return "ule";
//         case CmpInst::ICMP_SGT: return "sgt";
//         case CmpInst::ICMP_SGE: return "sge";
//         case CmpInst::ICMP_SLT: return "slt";
//         case CmpInst::ICMP_SLE: return "sle";
//         default: return "unknown";
//     }
// }

//   std::string get_predecessor_values_and_meet(BasicBlock *BB, Function &F,std::string value) {
//     bool found = false;
//                     // Check if the current block has multiple predecessors
                    
//                         std::vector<std::string> predValues;
//                         // Iterate through all predecessors
//                         for (auto *Pred : predecessors(BB)) {
//                             // Access the map of the predecessor block
//                             auto it = BlockVarValueMaps.find(Pred);
                            
//                             if (it != BlockVarValueMaps.end()) {
//                                 // Check if the value operand exists in the predecessor's map
//                                 if (it->second.find(value) != it->second.end()) {
//                                     // Update VarValueMap with the value from the predecessor's map
//                                     predValues.push_back(it->second[value]);
//                                     found = true;
                                
//                                 }
//                             }
//                         }
                   
//                         if (found) {
//                             // If the value operand is found in predecessor maps
//                             // Pass all the values to a take_meet function along with BlockVarValueMaps
//                             std::string meet_result = meet_operation(predValues);
//                             return meet_result;
                            
//                         }
//                         else{
//                              return "TOP";
//                         }
                        

//     }
    

// std::unordered_map<std::string, std::string> processBasicBlock_write_results(BasicBlock *BB, Function &F,std::unordered_map<std::string, std::string>& VarValueMap) {
//     std::unordered_map<std::string,std::string>local_map;
//     // static int block_count=0;
//     // block_count++;BranchInst

//     // outs()<<std::to_string(block_count)<<"\n";
//       for (auto &Inst : *BB) {
//          if (isa<AllocaInst>(Inst)) {

//           std::string instructionString;
//           raw_string_ostream os(instructionString);
//           Inst.print(os); // Print the instruction to a string
          
//           AllocaInst *Alloca = cast<AllocaInst>(&Inst);
//           std::string lhs= os.str();
//           std::string alloca_string;
//            raw_string_ostream (alloca_string) << *Alloca;
//            alloca_string=alloca_string.substr(0, alloca_string.find("="));
//            alloca_string.erase(std::remove_if(alloca_string.begin(), alloca_string.end(), ::isspace), alloca_string.end());
//           lhs += "  --> " + Alloca->getName().str();
//           ltrim(lhs);
//           lhs += "=TOP";     
//           VarValueMap[alloca_string]="TOP";  
//           results.push_back(lhs);
//           } 
        
//         if (isa<StoreInst>(&Inst)) {
//                 StoreInst *Store = cast<StoreInst>(&Inst);
//                 Value *ValueOperand = Store->getValueOperand();
//                 Value *PointerOperand = Store->getPointerOperand();
//                  std::string value_string;
//                  std::string pointer_string;
                 
//                 raw_string_ostream (value_string) <<*ValueOperand;
// 				raw_string_ostream (pointer_string) << *PointerOperand;
//                 value_string=value_string.substr(0, value_string.find("="));
//                 pointer_string=pointer_string.substr(0, pointer_string.find("="));
//                value_string.erase(std::remove_if(value_string.begin(), value_string.end(), ::isspace), value_string.end());
//                 pointer_string.erase(std::remove_if(pointer_string.begin(), pointer_string.end(), ::isspace), pointer_string.end());
//                 std::string instructionString;
//                 raw_string_ostream os(instructionString);
//                 Inst.print(os); // Print the instruction to a string
//                 std::string lhs= os.str();
//                 lhs +=  "  --> " + pointer_string;
//                 ltrim(lhs);

//                 if (ConstantInt *CI = dyn_cast<ConstantInt>(ValueOperand)) {
//                     // If the value operand is a constant integer
//                     std::string rhs = "="+ std::to_string(CI->getSExtValue());
//                     VarValueMap[pointer_string] = std::to_string(CI->getSExtValue());
                   
//                     local_map[pointer_string] = std::to_string(CI->getSExtValue());
//                     results.push_back(lhs+rhs);
//                 } else {
//                     // If the value operand is not a constant integer but a variable
//                     if (local_map.find(value_string) != local_map.end() &&
//                         local_map[value_string] != "TOP" ) {
//                         // If the value operand is already present in the current VarValueMap and its value is not TOP 
//                         VarValueMap[pointer_string] = local_map[value_string];
//                          std::string rhs = " = " + VarValueMap[pointer_string]+ "," + value_string +"="+VarValueMap[pointer_string];
//                          results.push_back(lhs + rhs);
//                     } 
//                     else if (local_map[value_string] == "BOTTOM"){
//                         VarValueMap[pointer_string]="BOTTOM";
//                         // VarValueMap[pointer_string] = local_map[value_string];
//                          std::string rhs = " = " + VarValueMap[pointer_string]+ "," + value_string +"="+VarValueMap[pointer_string];
//                          results.push_back(lhs + rhs);
//                     }
//                     else {
//                         // bool found = false;
//                          // Check if the current block has multiple predecessors
//                         if (BB->getSinglePredecessor() == nullptr) {
                        
//                          VarValueMap[pointer_string]=get_predecessor_values_and_meet(BB,F,value_string);
//                          local_map[pointer_string]=VarValueMap[value_string];
//                           std::string rhs = " = " + VarValueMap[pointer_string]+ "," + value_string+"="+VarValueMap[pointer_string];
//                          results.push_back(lhs + rhs);
//                         }
//                         else {
//                             // If the current block has only one predecessor, proceed as usual
//                             VarValueMap[pointer_string] = VarValueMap[value_string];
//                              local_map[pointer_string]=VarValueMap[value_string];
//                             std::string rhs = " = " + VarValueMap[pointer_string]+ "," + value_string +"="+VarValueMap[pointer_string];
//                             results.push_back(lhs + rhs);
//                         }
//                     }
//                 }
//             }


//         else if (BinaryOperator *BinOp = dyn_cast<BinaryOperator>(&Inst)) {
//             Value *Op1 = BinOp->getOperand(0);
//             Value *Op2 = BinOp->getOperand(1);
//             std::string operation= llvm::Instruction::getOpcodeName(Inst.getOpcode());

  

//                 // Get all operands in string format
//                 std::string op1String;
//                 raw_string_ostream (op1String) << *Op1;
            
                
//                 std::string op1 =op1String;
//                 ltrim(op1);
//                 op1=op1.substr(0, op1.find("="));
            
            

//             std::string op2String;
//                 raw_string_ostream (op2String) << *Op2;
//                 // Op2->print(os2);
            
//                 std::string op2 = op2String;
//                 ltrim(op2);
//                 op2=op2.substr(0, op2.find("="));

//             std::string dest_string;
//                 raw_string_ostream (dest_string) << *BinOp;
//                 ltrim(dest_string);
//                 //  outs()<<"BIN_DESTINATION:"<<dest_string<<"\n";
//                 dest_string=dest_string.substr(0, dest_string.find("="));
//                 op1.erase(std::remove_if(op1.begin(), op1.end(), ::isspace), op1.end());
//                 op2.erase(std::remove_if(op2.begin(), op2.end(), ::isspace), op2.end());
//                 dest_string.erase(std::remove_if(dest_string.begin(), dest_string.end(), ::isspace), dest_string.end());


//             std::string result;
//             std::string instructionString;
//             raw_string_ostream os(instructionString);
//             Inst.print(os); // Print the instruction to a string
//             std::string lhs = os.str();
//             lhs += "  --> " +dest_string;
//             ltrim(lhs);

//             std::string rhs;

//             if (ConstantInt *CI1 = dyn_cast<ConstantInt>(Op1)) {
//                 // If the first operand is a constant integer
//                 if (ConstantInt *CI2 = dyn_cast<ConstantInt>(Op2)) {
//                     // If both operands are constant integers
//                     // Perform the binary operation and store the resultassignment-2/assignment-2-constant-propagation-RajAmbekar191094/assign
//                     std::string val1 = std::to_string(CI1->getSExtValue());
//                     std::string val2 = std::to_string(CI2->getSExtValue());
//                     // std::string res;
                    
//                     result= evaluateBinaryOperation(val1,val2,operation);
//                     VarValueMap[dest_string] = result;
//                     rhs = result;
//                     // result = std::to_string(res);
//                 } else {
//                     // If the second operand is a variable
//                     if (local_map.find(op2) != local_map.end()) {
//                         // If the variable exists in VarValueMap, apply meet operation
//                         result = evaluateBinaryOperation(std::to_string(CI1->getSExtValue()), local_map[op2],operation);
//                         VarValueMap[dest_string] = result;
//                         local_map[dest_string]=result;
//                         rhs = result+", "+op2+"="+VarValueMap[op2];
//                     } else {
//                         // If the variable does not exist in local_map, it comes from predecessors. take meet
//                             //   bool found = false;
//                             // Check if the current block has multiple predecessors
//                         if (BB->getSinglePredecessor() == nullptr) {
//                             //     std::vector<std::string> predValues;
//                             //     // Iterate through all predecessors
//                             //     for (auto *Pred : predecessors(BB)) {
//                             //         // Access the map of the predecessor block
//                             //         auto it = BlockVarValueMaps.find(Pred);
//                             //         if (it != BlockVarValueMaps.end()) {
//                             //             // Check if the value operand exists in the predecessor's map
//                             //             if (it->second.find(op2) != it->second.end()) {
//                             //                 // Update VarValueMap with the value from the predecessor's map
//                             //                 predValues.push_back(it->second[op2]);
//                             //                 found = true;
//                             //             }
//                             //         }
//                             //     }
//                             //     if (found) {
//                             //         // If the value operand is found in predecessor maps
//                             //         // Pass all the values to a take_meet function along with BlockVarValueMaps
//                             //         std::string meet_result = meet_operation(predValues);
//                             //         VarValueMap[op2] = meet_result;
//                             //         VarValueMap[dest_string]= meet_result;
//                             //     }
//                         std::string op2_result=get_predecessor_values_and_meet(BB,F,op2);
                    
//                         VarValueMap[op2] = op2_result;
//                         local_map[op2]= op2_result;
                        
//                         result=evaluateBinaryOperation(std::to_string(CI1->getSExtValue()),op2_result,operation);
//                         VarValueMap[dest_string] = result;
//                         local_map[dest_string]= result;
//                         rhs = result+", "+op2+"="+VarValueMap[op2];
//                         }
//                         else{
//                             result=evaluateBinaryOperation(std::to_string(CI1->getSExtValue()),VarValueMap[op2],operation);
//                             rhs = result+", "+op2+"="+VarValueMap[op2];
//                         }
//                     //             } else {
//                     //                 // If the variable is not found in the predecessor blocks' maps
//                     //                 // Store the variable name without its value i.e TOP
//                     //                 VarValueMap[op2] = "TOP";
//                     //             }
//                     // }
//                 }
//                 }} 
//                 // If the first operand is a variable and second is constant
                
                
//             else if (ConstantInt *CI2 = dyn_cast<ConstantInt>(Op2)) {
//                     // If the second operand is a constant integer
//                     // Apply meet operation
//                         if (local_map.find(op1) != local_map.end()){
//                         // If the variable exists in VarValueMap, apply meet operation
//                         result = evaluateBinaryOperation(local_map[op1],std::to_string(CI2->getSExtValue()),operation);
//                         VarValueMap[dest_string] = result;
//                         local_map[dest_string]=result;
//                         rhs = result+", "+op1+"="+VarValueMap[op1];
//                         }
//                         else{
//                             // Check if the current block has multiple predecessors
//                         if (BB->getSinglePredecessor() == nullptr) {
//                                 std::string op1_result=get_predecessor_values_and_meet(BB,F,op1);
                                
//                                 VarValueMap[op1] = op1_result;
//                                 local_map[op1]= op1_result;
//                                 VarValueMap[dest_string] = evaluateBinaryOperation(op1_result,std::to_string(CI2->getSExtValue()),operation);
//                                 local_map[dest_string]=VarValueMap[dest_string];
//                                 rhs = VarValueMap[dest_string]+", "+op1+"="+VarValueMap[op1]; 
//                         }
                
//                         else{
//                             result=evaluateBinaryOperation(VarValueMap[op1],std::to_string(CI2->getSExtValue()),operation);
//                             rhs = result+", "+op1+"="+VarValueMap[op1];
                                
//                             }
//                 // }
//                         }
//                 } else {
//                     // If both the  operands are a variable
//                     std::string op1_result,op2_result;
//                     if (local_map.find(op1) != local_map.end() && local_map.find(op2) != local_map.end()){
//                         result = evaluateBinaryOperation(local_map[op1],local_map[op2],operation);
//                         VarValueMap[dest_string] = result;
//                         VarValueMap[op1] = local_map[op1];
//                         VarValueMap[op2] = local_map[op2];
//                         local_map[dest_string]=result;
//                         rhs = VarValueMap[dest_string]+", "+op1+"="+VarValueMap[op1]+" ,"+op2+"="+VarValueMap[op2];

//                 }BranchInst
//                     if (local_map.find(op1) == local_map.end() && local_map.find(op2) != local_map.end()){
//                         if (BB->getSinglePredecessor() == nullptr) {
//                         op1_result=get_predecessor_values_and_meet(BB,F,op1);
//                         }else{
//                             op1_result=VarValueMap[op1];
//                         }
//                         result = evaluateBinaryOperation(op1_result,local_map[op2],operation);
//                         VarValueMap[op1]=op1_result;
//                         local_map[op1]=op1_result;
//                         VarValueMap[dest_string] = result;
//                         local_map[dest_string]=result;
//                         rhs = VarValueMap[dest_string]+", "+op1+"="+VarValueMap[op1]+" ,"+op2+"="+VarValueMap[op2];

//                 }
//                     if (local_map.find(op1) != local_mBranchInstap.end() && local_map.find(op2) == local_map.end()){
//                         if (BB->getSinglePredecessor() == nullptr) {
//                         op2_result=get_predecessor_values_and_meet(BB,F,op2);
//                         }
//                         else{
//                             op2_result=VarValueMap[op2];
//                         }
//                         result = evaluateBinaryOperation(local_map[op1],op2_result,operation);
//                         VarValueMap[op2]=op2_result;
//                         local_map[op2]=op2_result;
//                         VarValueMap[dest_string] = result;
//                         local_map[dest_string]=result;
//                         rhs = VarValueMap[dest_string]+", "+op1+"="+VarValueMap[op1]+" ,"+op2+"="+VarValueMap[op2];
// BranchInst
//                 }
//                     if (local_map.find(op1) == local_map.end() && local_map.find(op2) == local_map.end()){
//                         if (BB->getSinglePredecessor() == nullptr) {
//                         bool op1Found = false, op2Found = false;
//                         std::vector<std::string> predMaps1;
//                         std::vector<std::string> predMaps2;
//                         std::vector<std::string> dest_operand;
//                         // Check each predecessor map for the presence of operands
//                         for (auto *Pred : predecessors(BB)) {
//                             auto it = BlockVarValueMaps.find(Pred);
//                             if (it != BlockVarValueMaps.end()) {
//                                 if (it->second.find(op1) != it->second.end()) {
//                                     op1Found = true;
//                                     predMaps1.push_back(it->second[op1]);
//                                 }
//                                 if (it->second.find(op2) != it->second.end()) {
//                                     op2Found = true;
//                                     predMaps2.push_back(it->second[op2]);
//                                 }
//                                 if(op1Found && op2Found){
//                                 std::string dest_val=evaluateBinaryOperation(it->second[op1],it->second[op2],operation) ;
//                                 dest_operand.push_back(dest_val);


//                                 }
//                             }
//                         }
//                         VarValueMap[op1]=meet_operation(predMaps1);
//                         VarValueMap[op2]=meet_operation(predMaps2);
//                         VarValueMap[dest_string] = meet_operation(dest_operand);
//                         local_map[op1]=meet_operation(BranchInstpredMaps1);
//                         local_map[op2]=meet_operation(predMaps2);
//                         local_map[dest_string] = meet_operation(dest_operand);
//                         rhs = VarValueMap[dest_string]+", "+op1+"="+VarValueMap[op1]+" ,"+op2+"="+VarValueMap[op2];

//                         }
//                         else{
//                             result=evaluateBinaryOperation(VarValueMap[op1],VarValueMap[op2],operation);
                    
//                         local_map[op1]=VarValueMap[op1];
//                         local_map[op2]=VarValueMap[op2];
//                         local_map[dest_string] = VarValueMap[op2];
//                         rhs = VarValueMap[dest_string]+", "+op1+"="+VarValueMap[op1]+" ,"+op2+"="+VarValueMap[op2];


//                         }BranchInst
//                 }

//                 }
//              results.push_back(lhs + "=" + rhs);
//             }

            


//             else if (LoadInst *Load = dyn_cast<LoadInst>(&Inst)) {

              
//                 //get operands in string format
//                 Value *PtrOperand = Load->getPointerOperand();
             
//                 std::string instructionString;
//                 raw_string_ostream os(instructionString);
//                 Inst.print(os); // Print the instruction to a string
//                 std::string lhs = os.str();
//                 ltrim(lhs);
//                 lhs += "  --> ";
//                 std::string ptr_string,val_string;
//                 raw_string_ostream (ptr_string) << *PtrOperand;
               
            
//                 val_string=lhs.substr(0, lhs.find("="));
//                 ptr_string=ptr_string.substr(0, ptr_string.find("="));
//                  val_string.erase(std::remove_if(val_string.begin(), val_string.end(), ::isspace), val_string.end());
//                  ptr_string.erase(std::remove_if(ptr_string.begin(), ptr_string.end(), ::isspace), ptr_string.end());
                
 
//                 if (local_map.find(ptr_string) != local_map.end() &&
//                     local_map[ptr_string] != "TOP"){
//                     // If the pointer operand is already present in the current VarValueMap and its value is not TOP or BOTTOM
//                      local_map[val_string] = local_map[ptr_string];
//                     VarValueMap[val_string] = local_map[ptr_string];
//                      std::string rhs= val_string+"="+VarValueMap[ptr_string]+ ", " +ptr_string+"="+VarValueMap[ptr_string];
//                 } else {
                    
                    
//                     // // Check if the current block has multiple predecessors
//                      std::string operand_result;
//                   if (BB->getSinglePredecessor() == nullptr) {
//                    operand_result=get_predecessor_values_and_meet(BB,F,ptr_string);
//                   }
//                   else{
//                     operand_result=VarValueMap[ptr_strBranchInsting];

//                   }
                 
//                     VarValueMap[val_string] = operand_result;
//                     VarValueMap[ptr_string] = operand_result;
//                     local_map[val_string] = operand_result;
//                     local_map[ptr_string] = operand_result;
//                 }
//                 std::string rhs= val_string+"="+VarValueMap[ptr_string]+ ", " +ptr_string+"="+VarValueMap[ptr_string];
//                 results.push_back(lhs+rhs);
//             }

//             if(CallInst *Call = dyn_cast<CallInst>(&Inst)){
//                 if (isa<CallInst>(&Inst)) {
//                     std::string dest_string;
//                     raw_string_ostream (dest_string) << *Call;
//                     ltrim(dest_string);
            
//              dest_string=dest_string.substr(0, dest_string.find("="));
//               dest_string.erase(std::remove_if(dest_string.begin(), dest_string.end(), ::isspace),dest_string.end());
//                 std::string instructionString;
//                 raw_string_ostream os(instructionString);
//                 Inst.print(os); // Print the instruction to a string
//                 std::string lhs= os.str();
                
//                 lhs += "  --> ";
//                 ltrim(lhs);
//                   Function *CalledFunc = Call->getCalledFunction();
//                   if (CalledFunc && CalledFunc->getName() == "__isoc99_scanf") {
//                       Value *Arg = Call->getArgOperand(1);
//                       std::string arg_string;
//                      raw_string_ostream (arg_string) << *Arg;
                     
                   
                   
//                     arg_string=arg_string.substr(0, arg_string.find("="));
//                     arg_string.erase(std::remove_if(arg_string.begin(), arg_string.end(), ::isspace),arg_string.end());
//                     //   if (Arg->hasName()) {
//                         VarValueMap[arg_string]="BOTTOM";
//                         local_map[arg_string]="BOTTOM";

                        

                      
//                     //   }
//                        lhs += Call->getName().str()+"="+VarValueMap[arg_string]+", %"+arg_string;
//                        results.push_back(lhs + "=BOTTOM");
//                   }

//                   if (CalledFunc && CalledFunc->getName() == "printf") {
//                       Value *Arg = Call->getArgOperand(1);
//                       // Assuming 'operand' is a pointer to the Value operand
//                       std::string arg_string;
//                      raw_string_ostream (arg_string) << *Arg;
                     
                   
                   
//                     arg_string=arg_string.substr(0, arg_string.find("="));
//                     arg_string.erase(std::remove_if(arg_string.begin(), arg_string.end(), ::isspace),arg_string.end());
                    
//                         lhs += dest_string+"="+VarValueMap[arg_string]+" ,"+arg_string+"="+VarValueMap[arg_string];

//                         results.push_back(lhs);
//                   }

        

//             }
//           }
//     if (isa<ICmpInst>(&Inst) ) {
//             std::string instructionString;
//             raw_string_ostream os(instructionString);
//             Inst.print(os); // Print the instruction to a string
//             std::string comp_inst= os.str();
//             ltrim(comp_inst);
        
//             ICmpInst *IC = dyn_cast<ICmpInst>(&Inst);
//             Value *Op1 = Inst.getOperand(0);
//             Value *Op2 = Inst.getOperand(1);
            
//             CmpInst::Predicate predicate = IC->getPredicate();
//                 std::string predicateStr = predicateToString(predicate);
//             //  outs()<<"ICMP PRED:"<<predicateStr<<"\n";
//             std::string operation= "icmp_"+predicateStr;
//             std::string result;
//                 std::string op1String;
//             raw_string_ostream (op1String) << *Op1;
//             // Op1->print(os1);
            
//             std::string op1 =op1String;
//             ltrim(op1);
//             op1=op1.substr(0, op1.find("="));
      
           

//            std::string op2String;
//             raw_string_ostream (op2String) << *Op2;
//             // Op2->print(os2);
           
//             std::string op2 = op2String;
//             ltrim(op2);
//             op2=op2.substr(0, op2.find("="));
     

//              std::string dest_string;
//             raw_string_ostream (dest_string) << *IC;
//              ltrim(dest_string);
//             //  outs()<<"BIN_DESTINATION:"<<dest_string<<"\n";
//              dest_string=dest_string.substr(0, dest_string.find("="));
//              op1.erase(std::remove_if(op1.begin(), op1.end(), ::isspace), op1.end());
//              op2.erase(std::remove_if(op2.begin(), op2.end(), ::isspace), op2.end());
//              dest_string.erase(std::remove_if(dest_string.begin(), dest_string.end(), ::isspace), dest_string.end());
//         if (ConstantInt *CI1 = dyn_cast<ConstantInt>(Op1)) {
//             // If the first operand is a constant integer
//             if (ConstantInt *CI2 = dyn_cast<ConstantInt>(Op2)) {
//                 // If both operands are constant integers
//                 // Perform the binary operation and store the result
//                 std::string val1 = std::to_string(CI1->getSExtValue());
//                 std::string val2 = std::to_string(CI2->getSExtValue());
                
                
//                 result= evaluateBinaryOperation(val1,val2,operation);
//                  results.push_back(comp_inst += dest_string+"=" +result);
//                 // result = std::to_string(res);
//             } else {
//                 // If the second operand is a variable
//                 if (local_map.find(op2) != local_map.end()) {
//                     // If the variable exists in VarValueMap, apply meet operation
//                     result = evaluateBinaryOperation(std::to_string(CI1->getSExtValue()), local_map[op2],operation);
//                     VarValueMap[dest_string] = result;
//                      results.push_back(comp_inst += dest_string+"="+result+" ,"+op2+"="+local_map[op2]);
//                 } else {
//                      // // Check if the current block has multiple predecessors
//                    if (BB->getSinglePredecessor() == nullptr) {
                    
//                     std::string op2_result=get_predecessor_values_and_meet(BB,F,op2);
                
//                     VarValueMap[op2] = op2_result;
//                     local_map[op2]= op2_result;
//                     VarValueMap[dest_string] = op2_result;
//                      result = evaluateBinaryOperation(std::to_string(CI1->getSExtValue()), local_map[op2],operation);
//                     VarValueMap[dest_string] = result;
//                      results.push_back(comp_inst += dest_string+"="+result+" ,"+op2+"="+local_map[op2]);
//                    }
//                    else{

//                     local_map[op2]= VarValueMap[op2];
//                      result = evaluateBinaryOperation(std::to_string(CI1->getSExtValue()), local_map[op2],operation);
//                       results.push_back(comp_inst += dest_string+"="+result+" ,"+op2+"="+local_map[op2]);

//                    }

//             }
            
//             }} 
//             // If the first operand is a variable and second is constant
            
            
//        else if (ConstantInt *CI2 = dyn_cast<ConstantInt>(Op2)) {
//                 // If the second operand is a constant integer
//                 // Apply meet operation
//                     if (local_map.find(op1) != local_map.end()){
//                     // If the variable exists in local map, no need of meet
//                     result = evaluateBinaryOperation(local_map[op1],std::to_string(CI2->getSExtValue()),operation);
//                     VarValueMap[dest_string] = result;
//                      VarValueMap[op1] =local_map[op1];
//                      results.push_back(comp_inst += " -->" + dest_string+"="+result+" ,"+op1+"="+local_map[op1]);
          

//                     }
//                     else{
//                          // Check if the current block has multiple predecessors
//                     if (BB->getSinglePredecessor() == nullptr) {
//                             std::string op1_result=get_predecessor_values_and_meet(BB,F,op1);
                            
//                             VarValueMap[op1] = op1_result;
//                             local_map[op1]= op1_result;
//                             VarValueMap[dest_string] = evaluateBinaryOperation(op1_result,std::to_string(CI2->getSExtValue()),operation);
//                             local_map[dest_string]=VarValueMap[dest_string];
//                             results.push_back(comp_inst += " -->" + dest_string+"="+result+" ,"+op1+"="+local_map[op2]);
//                     }
               
//                     else{
//                          result=evaluateBinaryOperation(VarValueMap[op1],std::to_string(CI2->getSExtValue()),operation);
//                         results.push_back(comp_inst += " -->" + dest_string+"="+result+" ,"+op1+"="+local_map[op2]);
                            
//                         }
//             // }
//                     }
//             } else {
//                 // If both the  operands are a variable
//                 std::string op1_result,op2_result;
//                 if (local_map.find(op1) != local_map.end() && local_map.find(op2) != local_map.end()){
//                     result = evaluateBinaryOperation(local_map[op1],local_map[op2],operation);
//                     VarValueMap[dest_string] = result;
//                     VarValueMap[op1] = local_map[op1];
//                     VarValueMap[op2] = local_map[op2];
//                     local_map[dest_string]=result;
//                    results.push_back(comp_inst += " -->" + dest_string+"="+result+" ,"+op1+"="+local_map[op1] +"," +op2+"="+local_map[op2]);

//             }
//                   if (local_map.find(op1) == local_map.end() && local_map.find(op2) != local_map.end()){
//                      if (BB->getSinglePredecessor() == nullptr) {
//                        op1_result=get_predecessor_values_and_meet(BB,F,op1);
//                      }else{
//                         op1_result=VarValueMap[op1];
//                      }
//                     result = evaluateBinaryOperation(op1_result,local_map[op2],operation);
//                     VarValueMap[op1]=op1_result;
//                     local_map[op1]=op1_result;
//                     VarValueMap[dest_string] = result;
//                     local_map[dest_string]=result;
//                     results.push_back(comp_inst += " -->" + dest_string+"="+result+" ,"+op1+"="+local_map[op1] +"," +op2+"="+local_map[op2]);

//             }
//                   if (local_map.find(op1) != local_map.end() && local_map.find(op2) == local_map.end()){
//                      if (BB->getSinglePredecessor() == nullptr) {
//                     op2_result=get_predecessor_values_and_meet(BB,F,op2);
//                      }
//                      else{
//                         op2_result=VarValueMap[op2];
//                      }
//                     result = evaluateBinaryOperation(local_map[op1],op2_result,operation);
//                     VarValueMap[op2]=op2_result;
//                     local_map[op2]=op2_result;
//                     VarValueMap[dest_string] = result;
//                     local_map[dest_string]=result;
//                     results.push_back(comp_inst += " -->" + dest_string+"="+result+" ,"+op1+"="+local_map[op1] +"," +op2+"="+local_map[op2]);

//             }
//                   if (local_map.find(op1) == local_map.end() && local_map.find(op2) == local_map.end()){
//                      if (BB->getSinglePredecessor() == nullptr) {
//                     bool op1Found = false, op2Found = false;
//                     std::vector<std::string> predMaps1;
//                     std::vector<std::string> predMaps2;
//                     std::vector<std::string> dest_operand;
//                     // Check each predecessor map for the presence of operands
//                     for (auto *Pred : predecessors(BB)) {
//                         auto it = BlockVarValueMaps.find(Pred);
//                         if (it != BlockVarValueMaps.end()) {
//                             if (it->second.find(op1) != it->second.end()) {
//                                 op1Found = true;
//                                 predMaps1.push_back(it->second[op1]);
//                             }
//                             if (it->second.find(op2) != it->second.end()) {
//                                 op2Found = true;
//                                 predMaps2.push_back(it->second[op2]);
//                             }
//                             if(op1Found && op2Found){
//                             std::string dest_val=evaluateBinaryOperation(it->second[op1],it->second[op2],operation) ;
//                             dest_operand.push_back(dest_val);


//                             }
//                         }
//                     }
//                       VarValueMap[op1]=meet_operation(predMaps1);
//                       VarValueMap[op2]=meet_operation(predMaps2);
//                       VarValueMap[dest_string] = meet_operation(dest_operand);
//                       local_map[op1]=meet_operation(predMaps1);
//                       local_map[op2]=meet_operation(predMaps2);
//                       local_map[dest_string] = meet_operation(dest_operand);
//                       results.push_back(comp_inst += " -->" + dest_string+"="+result+" ,"+op1+"="+local_map[op1] +"," +op2+"="+local_map[op2]);

//                      }
//                       else{
//                         result=evaluateBinaryOperation(VarValueMap[op1],VarValueMap[op2],operation);
                  
//                       local_map[op1]=VarValueMap[op1];
//                       local_map[op2]=VarValueMap[op2];
//                       local_map[dest_string] = VarValueMap[op2];
//                       results.push_back(comp_inst += " -->" + dest_string+"="+result+" ,"+op1+"="+local_map[op1] +"," +op2+"="+local_map[op2]);


//                       }
//             }

//             }
            
//     }
//      // Check if the instruction is a branch instruction (br)
//         else if (isa<BranchInst>(&Inst)) {
//                 std::string instructionString;
//                 raw_string_ostream os(instructionString);
//                 Inst.print(os); // Print the instruction to a string
//                 std::string branch_inst= os.str();
//                  ltrim(branch_inst);
//                  results.push_back(branch_inst);
//                  results.push_back("");
//                 //  outs()<<std::to_string(block_count)<<"\n";
//             }
// }
 

 
//   // }
//   return VarValueMap;
//   }



// bool runOnFunction(Function &F) override {

//      std::unordered_map<BasicBlock*, std::unordered_map<std::string, std::string>> BlockVarValueMaps;
    
//     std::unordered_map<std::string, std::string> current_map;
//     std::unordered_map<std::string, std::string> new_map;
//     std::queue<BasicBlock *> worklist;
   


//  //push entry block and then   all other basic blocks into a worklist
   
   
//  worklist.push(&F.getEntryBlock());


// // Initialize worklist with all basic blocks except the entry block
//    for (auto &BB : F) {
     
//         if (&BB == &F.getEntryBlock()){
//                 continue;
//         }
//         else{  
//         worklist.push(&BB);
//         }
//     }

    

//     do{
   
      
//         BasicBlock *CurrBlock = worklist.front();
//          worklist.pop();
        
//         if(CurrBlock->isEntryBlock() ){
   
//          new_map =processBasicBlock_write_results(CurrBlock, F,current_map);
      
//           BlockVarValueMaps[CurrBlock]=new_map;
     

//         }
       
     
        

//         if (CurrBlock->getSinglePredecessor() != nullptr) {
 
//             new_map = processBasicBlock_write_results(CurrBlock, F, current_map);
//             BlockVarValueMaps[CurrBlock] = new_map;


//         } else {
//             // Multiple predecessors, use meet_and_append
//             // Iterate through predecessors
// 			std::unordered_map<std::string, std::string> unionMap;
//             for (auto *Pred : predecessors(CurrBlock)) {
//                 auto it = BlockVarValueMaps.find(Pred);
//                 if (it != BlockVarValueMaps.end()) {
//                     // Iterate through the map entries of the predecessor
//                     for (const auto &entry : it->second) {
//                         // Check if the key already exists in the union map
//                         auto it_union = unionMap.find(entry.first);
//                         if (it_union == unionMap.end()) {
//                             // Key doesn't exist in union map, add it
//                             unionMap[entry.first] = entry.second;
//                         } else {
//                             // Key exists in union map, compare values
//                             if(entry.second =="BOTTOM" || it_union->second=="BOTTOM"){
//                                 it_union->second = "BOTTOM";
//                             }
//                             else if (entry.second != it_union->second) {
//                                 // If values are different, mark the key as BOTTOM
//                                 it_union->second = "BOTTOM";
                                
//                             }
//                             else if (entry.second == it_union->second){
//                                 continue;
//                             }
//                             else if (entry.second == "TOP" || it_union->second=="TOP"){
//                                 it_union->second = "TOP";
//                             }
                            
                            
//                         }
                        
                    
//                     }
//                 }
//             }
// 			current_map=unionMap;
//           new_map=processBasicBlock_write_results(CurrBlock, F, current_map);
//           BlockVarValueMaps[CurrBlock] = new_map;
           

//         }
          

//         if (!compare_maps(new_map, current_map)) {
//             // If there is a change, enqueue successors
//             current_map=new_map;
//             for (auto *Succ : successors(CurrBlock)) {
//                 worklist.push(Succ);
//             }
//         }
       


//     }while (!worklist.empty());
   


//      exportResultsToFile("output.txt");

//     return false;
//   }



// //   bool runOnFunction(Function &F) override {
   
  
// //     std::queue<BasicBlock *> BBQueue;
// //     std::unordered_set<BasicBlock *> visited;
// //     // std::unordered_map<std::string, std::string> current_VarValueMap;
// //     std::unordered_map<std::string, std::string> next_VarValueMap;
// //     //   std::unordered_map<BasicBlock *, unsigned> visitCount;

   

// //     // Process the entry block
// //     BasicBlock *EntryBlock = &F.getEntryBlock();
// //     // visitCount[EntryBlock] = 1; // Set visit count for the entry block to 1
// //     next_VarValueMap = processBasicBlock_write_results(EntryBlock, F,next_VarValueMap);
// //     BlockVarValueMaps[EntryBlock] = next_VarValueMap;

// //     visited.insert(EntryBlock);

// //     // Enqueue the entry block's successors
// //     for (auto *Succ : successors(EntryBlock)) {

// //         BBQueue.push(Succ);
// //         visited.insert(Succ);
// //         // visitCount[Succ]++;
// //     }

// //     // Process each basic block in BFS order
// //     while (!BBQueue.empty()) {
// //         BasicBlock *CurrBlock = BBQueue.front();
// //         BBQueue.pop();
// //         //  visitCount[CurrBlock]++;

// //            // If the block has been visited thrice, continue to the next block
// //         // if (visitCount[CurrBlock] > 2)
// //         //     continue;
 
// //         // Process the current basic block and get the next VarValueMap
// //         if (CurrBlock->getSinglePredecessor() != nullptr) {
// //           // If the block has a single predecessor, write results and update map
// //           next_VarValueMap = processBasicBlock_write_results(CurrBlock, F, next_VarValueMap);
// //           BlockVarValueMaps[CurrBlock] = next_VarValueMap;
// //         } else {
// //           // If the block has multiple predecessors, take union of maps and then update with end block
// //           std::unordered_map<std::string, std::string> unionMap;
    
            
// //             // Iterate through predecessors
// //             for (auto *Pred : predecessors(CurrBlock)) {
// //                 auto it = BlockVarValueMaps.find(Pred);
// //                 if (it != BlockVarValueMaps.end()) {
// //                     // Iterate through the map entries of the predecessor
// //                     for (const auto &entry : it->second) {
// //                         // Check if the key already exists in the union map
// //                         auto it_union = unionMap.find(entry.first);
// //                         if (it_union == unionMap.end()) {
// //                             // Key doesn't exist in union map, add it
// //                             unionMap[entry.first] = entry.second;
// //                         } else {
// //                             // Key exists in union map, compare values
// //                             if(entry.second =="BOTTOM" || it_union->second=="BOTTOM"){
// //                                 it_union->second = "BOTTOM";
// //                             }
// //                             else if (entry.second != it_union->second) {
// //                                 // If values are different, mark the key as BOTTOM
// //                                 it_union->second = "BOTTOM";
                                
// //                             }
// //                             else if (entry.second == it_union->second){
// //                                 continue;
// //                             }
// //                             else if (entry.second == "TOP" || it_union->second=="TOP"){
// //                                 it_union->second = "TOP";
// //                             }
                            
                            
// //                         }
                        
                    
// //                     }
// //                 }
// //             }
// //           next_VarValueMap=processBasicBlock_write_results(CurrBlock, F, unionMap);
// //           BlockVarValueMaps[CurrBlock] = next_VarValueMap;
// //         //   for (auto &entry : BlockVarValueMaps[CurrBlock]) {
// //         //         outs() << entry.first << " : " << entry.second << "\n";
// //         //     }
// //         //   BlockVarValueMaps[CurrBlock] = processBasicBlock_write_results(CurrBlock, F, BlockVarValueMaps[CurrBlock],multi_pred=1);
// //         }

// //         // Enqueue unvisited successor blocks
// //         for (auto *Succ : successors(CurrBlock)) {
// //              if (visited.find(Succ) == visited.end()) {
// //             // if (visitCount[Succ] < 2) {
// //                 BBQueue.push(Succ);
// //                 visited.insert(Succ);
// //                 }
// //             // }
// //         }

// //         // Update next_VarValueMap for the next iteration
// //         // next_VarValueMap = blockVarValueMap;
// //     }

// //     // Visit all remaining basic blocks (unreachable blocks)
// //     for (auto &BB : F) {
// //         if (visited.find(&BB) == visited.end()) {
// //             next_VarValueMap=processBasicBlock_write_results(&BB, F, next_VarValueMap);
// //             visited.insert(&BB);
// //         }
// //     }



// //       exportResultsToFile("output.txt");
  


// //     return false;
// // }
// }; // end of struct constant_p

// std::ofstream outputFile("output.txt");


// }  // end of anonymous namespace

// char constant_p::ID = 0;
// static RegisterPass<constant_p> X("constant_p", "Constant Propagation Pass for Assignment",
//                              false /* Only looks at CFG */,
//                              false /* Analysis Pass */);