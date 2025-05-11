#include "llvm/Pass.h"
#include "llvm/IR/Module.h" 
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instructions.h" 
#include "llvm/IR/LegacyPassManager.h"
#include <vector>
#include <fstream>
#include "llvm/IR/Constants.h"
#include "unordered_map"
#include <queue>
#include <unordered_set>
#include <stdexcept>
#include <sstream>
#include "intra_proc.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include <iostream>
// #include "llvm/Support/Error.h


// Your code here
extern std::string processString(const std::string& input) ;



using namespace llvm;
//extern 
// struct CallSite {
//     Function* functionPointer; // Function pointer
//     std::unordered_map<std::string, int> argumentValues; // Argument values
    
// };
// std::queue<pair<Function*, std::vector<std::string, int>>> call_site_data;   
std::unordered_map<llvm::Function*, std::unordered_map<std::string, std::string>> call_site_data;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              ;
std::queue<llvm::Function*> Visited;
extern std::unordered_map<llvm::Function*, std::unordered_map<std::string, std::pair<std::string,int>>> Function_constants_data;

namespace {


        void runTransformation(Function *F, Module &M) {
            // Iterate over all instructions in the function
            std::vector<Instruction *> dels_load;
            std::vector<Instruction *> dels_bin;
            std::vector<Instruction *> dels_store;
            std::vector<Instruction *> dels_alloca;
            // std::vector<Instruction *> dels;
            for (BasicBlock &BB : *F) {
                for (Instruction &I : BB) {
                     if (isa<AllocaInst>(I)) {


                                std::string instructionString;
                                raw_string_ostream os(instructionString);
                                I.print(os); // Print the instruction to a string
                                
                                AllocaInst *Alloca = cast<AllocaInst>(&I);
                                std::string lhs= os.str();
                                std::string alloca_string;
                                raw_string_ostream (alloca_string) << *Alloca;
                                alloca_string=alloca_string.substr(0, alloca_string.find("="));
                                alloca_string.erase(std::remove_if(alloca_string.begin(), alloca_string.end(), ::isspace), alloca_string.end());
                                 if (Function_constants_data[F][alloca_string].first != "BOTTOM" &&
                        Function_constants_data[F][alloca_string].first != "TOP") {
                             LLVMContext &Context = F->getContext();
                            ConstantInt *NewVal = ConstantInt::get(Type::getInt32Ty(Context),
                                                                safe_stoi(Function_constants_data [F][alloca_string].first));
                            // Replace all uses of the value operand with the constant value
                            // Alloca->replaceAllUsesWith(NewVal);
                            // for (auto& U : Alloca->uses()) {
                            //     Instruction *UserInst = cast<Instruction>(U.getUser());
                            //     if(UserInst){
                            //         U.set(NewVal);
                            //     }
                                
                            // }
                            //  Alloca->eraseFromParent();
                             dels_alloca.push_back(&I);
                        }

                     }
                     if (LoadInst *Load = dyn_cast<LoadInst>(&I)) {

              
                                //get operands in string format
                                Value *PtrOperand = Load->getPointerOperand();
                            
                                std::string instructionString;
                                raw_string_ostream os(instructionString);
                                I.print(os); // Print the instruction to a string
                                std::string lhs = os.str();
                                ltrim(lhs,"\t");
                                lhs += "  --> ";
                                std::string ptr_string,val_string;
                                raw_string_ostream (ptr_string) << *PtrOperand;
                            
                            
                                val_string=lhs.substr(0, lhs.find("="));
                                ptr_string=ptr_string.substr(0, ptr_string.find("="));
                                val_string.erase(std::remove_if(val_string.begin(), val_string.end(), ::isspace), val_string.end());
                                ptr_string.erase(std::remove_if(ptr_string.begin(), ptr_string.end(), ::isspace), ptr_string.end());
                                  if (Function_constants_data[F][ptr_string].first != "BOTTOM" &&
                                        Function_constants_data[F][ptr_string].first != "TOP") {
                                        LLVMContext &Context = F->getContext();
                                         ConstantInt *NewVal = ConstantInt::get(Type::getInt32Ty(Context),safe_stoi(Function_constants_data [F][ptr_string].first));
                                        // Replace all uses of the value operand with the constant value
                                        Load->replaceAllUsesWith(NewVal);
                                        // Load->eraseFromParent();
                                        dels_load.push_back(&I);
                        }
                    }
                    // Check if the instruction is a store instruction
                     if (isa<StoreInst>(&I)) {
                        bool ispointerconst=0;
                        bool isvalueconst=0;

                                StoreInst *Store = cast<StoreInst>(&I);
                                Value *Ptr = Store->getPointerOperand();
                                Value *Val = Store->getValueOperand();

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

                                if (Function_constants_data[F][pointer_string].first != "BOTTOM" &&
                        Function_constants_data[F][pointer_string].first != "TOP") {
                            ispointerconst=1;
                        LLVMContext &Context = M.getContext();
                        // Create a new constant value based on the stored constant value
                        ConstantInt *NewVal = ConstantInt::get(Type::getInt32Ty(Context),safe_stoi(Function_constants_data[F][pointer_string].first));
                        // Replace the operand with the new constant value

                        Store->setOperand(0, NewVal);
                        //  Store->replaceAllUsesWith(NewVal);
                        
                    }

                    if (Function_constants_data[F][value_string].first != "BOTTOM" &&
                        Function_constants_data[F][value_string].first != "TOP") {
                            isvalueconst=1;
                        // LLVMContext &Context = M.getContext();
                        // // Create a new constant value based on the stored constant value
                        // ConstantInt *NewVal = ConstantInt::get(Type::getInt32Ty(Context),
                        //                                        safe_stoi(Function_constants_data[F][value_string].first));
                        // // Replace the operand with the new constant value
                        // Store->setOperand(1, NewVal);
                    }
                    if(ispointerconst && isvalueconst){
                        //  Store->eraseFromParent();
                        dels_store.push_back(&I);
                    }
                }


            else if (BinaryOperator *BinOp = dyn_cast<BinaryOperator>(&I)) {
           
                Value *Op1 = BinOp->getOperand(0);
                Value *Op2 = BinOp->getOperand(1);
                std::string operation= llvm::Instruction::getOpcodeName(I.getOpcode());

  

                // Get all operands in string format
                std::string op1String;
                raw_string_ostream (op1String) << *Op1;
            
                
                std::string op1 =op1String;
                ltrim(op1,"\t");
                op1=op1.substr(0, op1.find("="));
            
            

                std::string op2String;
                raw_string_ostream (op2String) << *Op2;
                // Op2->print(os2);
            
                std::string op2 = op2String;
                ltrim(op2,"\t");
                op2=op2.substr(0, op2.find("="));

                std::string dest_string;
                raw_string_ostream (dest_string) << *BinOp;
                ltrim(dest_string,"\t");
                //  outs()<<"BIN_DESTINATION:"<<dest_string<<"\n";
                dest_string=dest_string.substr(0, dest_string.find("="));
                op1.erase(std::remove_if(op1.begin(), op1.end(), ::isspace), op1.end());
                op2.erase(std::remove_if(op2.begin(), op2.end(), ::isspace), op2.end());
                dest_string.erase(std::remove_if(dest_string.begin(), dest_string.end(), ::isspace), dest_string.end());
                if (Function_constants_data[F][dest_string].first != "BOTTOM" &&
                        Function_constants_data[F][dest_string].first != "TOP") {
                            LLVMContext &Context = F->getContext();
                    ConstantInt *NewVal = ConstantInt::get(Type::getInt32Ty(Context),
                                                           safe_stoi(Function_constants_data [F][dest_string].first));
                    // Replace all uses of the value operand with the constant value
                    BinOp->replaceAllUsesWith(NewVal);
                    dels_bin.push_back(&I);
                    
                    }
                    

            }


            if(CallInst *Call = dyn_cast<CallInst>(&I)){
                if (isa<CallInst>(&I)) {
                        Function *Callee = Call->getCalledFunction();
                          if (Callee->getName().find("llvm.") != 0  && Callee->getName() !="printf" && Callee->getName() !="__isoc99_scanf") {
                        for (const Argument &Arg : Callee->args()) {
                            Value *CallArg = Call->getArgOperand(Arg.getArgNo());
                            
                            if(call_site_data[Callee]["%"+Arg.getName().str()]!= "BOTTOM" &&
                        call_site_data[Callee]["%"+Arg.getName().str()] != "TOP" ){
                            LLVMContext &Context = F->getContext();
                            ConstantInt *NewVal = ConstantInt::get(Type::getInt32Ty(Context),safe_stoi(call_site_data[Callee]["%"+Arg.getName().str()]));
                            if(Callee->getName().str()=="fun" || Callee->getName().str()=="bar" ){
                                outs()<<safe_stoi(call_site_data[Callee]["%"+Arg.getName().str()])<<"\n";
                            }
                        Call->setArgOperand(Arg.getArgNo(), NewVal);

                        }
                    }
                }
              
                }
             }
        }
     }
         for (auto &I : dels_store) {
                I->eraseFromParent();
    }
       for (auto &I : dels_bin) {
                I->eraseFromParent();
    }
       for (auto &I : dels_load) {
                I->eraseFromParent();
    }
    //     for (auto &I : dels_alloca) {
    //             I->eraseFromParent();
    // }
// for (auto I = dels.rbegin(); I != dels.rend(); ++I) {
//     (*I)->eraseFromParent();
// }
}
           

    

    
    struct InterProceduralConstantPropagation : public ModulePass {
        static char ID; // Pass identification, replacement for typeid

        InterProceduralConstantPropagation() : ModulePass(ID) {}

        // Function to run the pass on the module
        bool runOnModule(Module &M) override {
            // Initialize data structures for inter-procedural analysis
        
            // Add all functions in the module to the worklist
             Function *MainFunc = M.getFunction("main");
            if (!MainFunc) {
                llvm::errs() << "Main function not found!\n";
                return false;
            }
            Visited.push(MainFunc);

            // Insert other functions into the visited set
            // for (auto &F : M) {
            //     if (&F != MainFunc) {
            //         Visited.push(&F);
            //     }
            // }

             do{
                Function *CurrFunc = Visited.front();
                Visited.pop();
                std::string functionName = CurrFunc->getName().str();
                if (functionName.find("llvm.") == 0|| functionName == "printf" || functionName == "__isoc99_scanf") {
                            // Skip processing functions starting with "llvm."
                            continue;
                 }
                 outs()<<"Running: "<<CurrFunc->getName().str()<<"\n";

                runIntraProceduralCP(*CurrFunc);
               


             }while (!Visited.empty());
            std::cout << "Visited:" << std::endl;
            std::queue<llvm::Function*> tempQueue = Visited;
            while (!tempQueue.empty()) {
                llvm::Function* func = tempQueue.front();
                tempQueue.pop();
                std::cout << "    Function: " << func->getName().str() << std::endl;
            }

            // Print the contents of call_site_data
            std::cout << "call_site_data:" << std::endl;
            for (const auto& entry : call_site_data) {
                llvm::Function* func = entry.first;
                std::cout << "Function: " << func->getName().str() << std::endl;
                const auto& innerMap = entry.second;
                for (const auto& innerEntry : innerMap) {
                    const std::string& arg = innerEntry.first;
                    const std::string& value = innerEntry.second;
                    std::cout << "    Argument: " << arg << ", Value: " << value << std::endl;
                }
            }

            // Print the contents of Function_constants_data
                std::cout << "Function_constants_data:" << std::endl;
                for (const auto& entry : Function_constants_data) {
                    llvm::Function* func = entry.first;
                    std::cout << "Function: " << func->getName().str() << std::endl;
                    const auto& innerMap = entry.second;
                    for (const auto& innerEntry : innerMap) {
                        const std::string& var = innerEntry.first;
                        const std::pair<std::string, int>& data = innerEntry.second;
                        const std::string& value = data.first;
                        int lineNumber = data.second;
                        std::cout << "    Variable: " << var << ", Value: " << value << ", Line Number: " << lineNumber << std::endl;
                    }
                }



     //Transformation of IR

        for (Function &F : M) {
                if (F.isDeclaration())
                    continue; // Skip external function declarations

                // Apply transformations based on function_constant_data and call_site_data
                runTransformation(&F,M);

                
                
            }
         // Write the modified LLVM IR to a new file
                    std::error_code ec;
                    // raw_fd_ostream outFile("transformed.ll", ec, sys::fs::F_Text);
                    // raw_fd_ostream outFile("transformed.ll", ec, llvm::sys::fs::OF_Text);
                    raw_fd_ostream outFile("transformed.ll", ec);

                    if (!ec) {
                        M.print(outFile, nullptr);
                        outFile.close();
                    } else {
                        errs() << "Error: Could not open file for writing.\n";
                    }
    return true; // This pass does modify the module
        }
          
    };
}
char InterProceduralConstantPropagation::ID = 0;
static RegisterPass<InterProceduralConstantPropagation> X("cons_eval", "Inter-Procedural Constant Propagation Pass");