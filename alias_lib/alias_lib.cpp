/* Assignment 3 : Alias Analysis
Author : Raj Ambekar (CS22MTECH12008)
Submitted on : 11th March 2024*/


#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"


#include "llvm/IR/BasicBlock.h"

#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/CFG.h"


#include "llvm/IR/LegacyPassManager.h"

#include <vector>
#include <fstream>
#include "llvm/IR/Constants.h"
#include "unordered_map"
#include <queue>
#include <unordered_set>
#include <stdexcept>
#include <sstream>

using namespace llvm;

namespace {
//function to compare two maps
bool compare_maps(const std::unordered_map<std::string,std::unordered_set<std::string>>& map1, const std::unordered_map<std::string, std::unordered_set<std::string>>& map2) {
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



std::unordered_map<std::string, std::unordered_set<std::string>> meet_and_append(const std::vector<std::unordered_map<std::string, std::unordered_set<std::string>>>& predecessor_maps, const std::unordered_map<std::string, std::unordered_set<std::string>>& current_map) {
    std::unordered_map<std::string, std::unordered_set<std::string>> result;

    // If the current map is empty, return an empty result
    if (current_map.empty()) {
        return result;
    }
    // outs()<<"CURRENT MAP"<<"\n";
    // printMap(current_map);

    // Union of values from all predecessors
    for (const auto& pred_map : predecessor_maps) {
        // If a predecessor map is empty, continue to the next one
        if (pred_map.empty()) {
            continue;
        }
        for (const auto& pair : pred_map) {
            // If the key already exists in result, insert values from pred_map
            if (result.find(pair.first) != result.end()) {
                for (const auto& value : pair.second) {
                    result[pair.first].insert(value);
                }
            } else { // Otherwise, directly insert the pair
                result.insert(pair);
            }
        }
    }


    // Append or overwrite keys with current map values
    for (const auto& pair : current_map) {
        // If the key already exists in result, insert values from current_map
        if (result.find(pair.first) != result.end()) {
            // result[pair.first].clear(); // Clear the set
            for (const auto& value : pair.second) {
                result[pair.first].insert(value);
            }
        } else { // Otherwise, directly insert the pair
            result.insert(pair);
        }
    }


    return result;
}




void exportResultsToTerminal(const std::string& filename,const std::string& functionName, const std::unordered_map<std::string, std::unordered_set<std::string>>& results) {

        errs() << "Function: " << functionName << "\n";
        

        //compute the aliases
    
      std::unordered_map<std::string, std::unordered_set<std::string>> aliasesMap;
      for (const auto& entry : results) {
        const std::string& pointer = entry.first;
        const std::unordered_set<std::string>& pointees = entry.second;

        // Iterate over all other pointer variables
        for (const auto& otherEntry : results) {
            const std::string& otherPointer = otherEntry.first;
            const std::unordered_set<std::string>& otherPointees = otherEntry.second;

            // Check if the current pointer and the other pointer are different
            if (pointer != otherPointer) {
                // Check if there is at least one common element between the sets of pointees
              
                bool hasCommonElement = false;
                for (const auto& pointee : pointees) {
                    if (otherPointees.find(pointee) != otherPointees.end()) {
                        hasCommonElement = true;
                       
                        break;
                    }
                }

                // If there is a common element, they are aliases
                if (hasCommonElement) {
                    aliasesMap[pointer].insert(otherPointer);
             
                    aliasesMap[otherPointer].insert(pointer);
                  
                   
                }
               
            }
        }
        if (aliasesMap.find(pointer) == aliasesMap.end()) {
        aliasesMap[pointer] = {};
    }
    }

 

    //print aliases
    errs() << "Aliases Map:\n";
    for (const auto& entry : aliasesMap) {
        errs() << entry.first << " -> {";
        bool first = true;
        if (entry.second.empty()) {
            errs() << " ";
        } else {
            for (const auto& alias : entry.second) {
                if (!first) {
                    errs() << ", ";
                }
                errs() << alias;
                first = false;
            }
        }
        errs() << "}\n";
    }
            
  
}


std::unordered_map<std::string, std::unordered_set<std::string>>get_aliases(BasicBlock *BB,  std::unordered_map<std::string, std::unordered_set<std::string>>pointerMap,Function &F){

 
//Loop over every instruction in a bsic block
      for (Instruction &I : *BB) {
        if (StoreInst *storeInst = dyn_cast<StoreInst>(&I)) {
           std::string value_string,pointer_string;
          // Check if the stored value is a pointer
   
          if (storeInst->getPointerOperand()->getType()->getPointerElementType()->isPointerTy()) {
     

            Value *pointerVar = storeInst->getPointerOperand();
            Value *storedValue = storeInst->getValueOperand();
            raw_string_ostream (pointer_string) << *pointerVar;
            raw_string_ostream (value_string) << *storedValue;
            value_string=value_string.substr(0, value_string.find("="));
            pointer_string=pointer_string.substr(0, pointer_string.find("="));

            value_string.erase(std::remove_if(value_string.begin(), value_string.end(), ::isspace), value_string.end());
            pointer_string.erase(std::remove_if(pointer_string.begin(), pointer_string.end(), ::isspace), pointer_string.end());

            if (LoadInst *loadInst = dyn_cast<LoadInst>(storedValue)) {                         //CASE  OF DIRECT POINTER ASSIGNMENT x=y
                // Get the name of the loaded pointer variable
                //get operands in string format
                 if (LoadInst *loadInst2 = dyn_cast<LoadInst>(pointerVar)) {    
                    // outs()<<"MAY MUST"<<"\n"; 
                    Value *PtrOperand1 = loadInst->getPointerOperand();
                    Value *PtrOperand2 = loadInst2->getPointerOperand();  
                    std::string load_ptr_string1,load_ptr_string2;
                    raw_string_ostream (load_ptr_string2) << *PtrOperand2;                        //CASE OF *x=z
                    raw_string_ostream (load_ptr_string1) << *PtrOperand1; 
                    load_ptr_string1=load_ptr_string1.substr(0, load_ptr_string1.find("="));
                    load_ptr_string2=load_ptr_string2.substr(0, load_ptr_string2.find("="));
                    load_ptr_string1.erase(std::remove_if(load_ptr_string1.begin(), load_ptr_string1.end(), ::isspace), load_ptr_string1.end());
                    load_ptr_string2.erase(std::remove_if(load_ptr_string2.begin(), load_ptr_string2.end(), ::isspace), load_ptr_string2.end());
      
                

                    if(pointerMap[load_ptr_string2].size()==1){
                     
                                pointerMap[*pointerMap[load_ptr_string2].begin()]= pointerMap[load_ptr_string1]; //Gen + kill --> strong update 
                             
                     

                    }else
                    {
                        
                                  
                            for (const auto& element : pointerMap[load_ptr_string2]) {                                                      //GEN , NO KILL---> WEAK UPDATE
                                pointerMap[element].insert(pointerMap[load_ptr_string1].begin(), pointerMap[load_ptr_string1].end());
                            }



                    }
                    

                       

                 }
                 else{

                Value *PtrOperand = loadInst->getPointerOperand();
                        if(LoadInst *loadInst1 = dyn_cast<LoadInst>(PtrOperand)){ 
                                                                                                     //CASE OF x=*z
                                    Value *PtrOperand1 = loadInst1->getPointerOperand();
                                    std::string ptr_string1,val_string1;
                                    raw_string_ostream (ptr_string1) << *PtrOperand1;
                                
                                
                                
                                    ptr_string1=ptr_string1.substr(0, ptr_string1.find("="));
                                    
                                    ptr_string1.erase(std::remove_if(ptr_string1.begin(), ptr_string1.end(), ::isspace), ptr_string1.end());
                                   

                                    pointerMap[pointer_string]=pointerMap[*pointerMap[ptr_string1].begin()];
                                  
                            

                        }
                        else{                                                                   //case of x=y
            
                    
            
                        std::string ptr_string,val_string;
                        raw_string_ostream (ptr_string) << *PtrOperand;
                    
                    
                    
                        ptr_string=ptr_string.substr(0, ptr_string.find("="));
                        
                        ptr_string.erase(std::remove_if(ptr_string.begin(), ptr_string.end(), ::isspace), ptr_string.end());


                        // Update the pointerMap accordingly
                        pointerMap[pointer_string]=(pointerMap[ptr_string]);
                       
                        }
                      
                        }
                    }

            else{                                                          

            if(LoadInst *loadInst = dyn_cast<LoadInst>(pointerVar)) {        //case of *x=&y
            Value *ptr_operand=loadInst->getPointerOperand();
            std::string load_ptr_string1;
            raw_string_ostream (load_ptr_string1) << *ptr_operand; 
             load_ptr_string1=load_ptr_string1.substr(0, load_ptr_string1.find("="));
             load_ptr_string1.erase(std::remove_if(load_ptr_string1.begin(), load_ptr_string1.end(), ::isspace), load_ptr_string1.end());

             if(pointerMap[load_ptr_string1].size()==1){
                        // outs()<<"!!!!"<<"\n";
                                pointerMap[*pointerMap[load_ptr_string1].begin()]= {value_string}; //Gen + kill --> strong update 
                            

                    }else
                    {
                        

                                    for (const auto& element : pointerMap[load_ptr_string1]) {                                                      //GEN , NO KILL---> WEAK UPDATE
                                        pointerMap[element].insert(value_string);
                                    }


//
                    }

            }
            else{

                    if (GetElementPtrInst *GEPInst = dyn_cast<GetElementPtrInst>(storedValue)) {  //case of array pointer 
                        // Get the operand of the getelementptr instruction
                        Value *arrayOperand = GEPInst->getOperand(0);
                        std::string arrayName;
                        raw_string_ostream(arrayName) << *arrayOperand;
                        arrayName = arrayName.substr(0, arrayName.find("="));
                        arrayName.erase(std::remove_if(arrayName.begin(), arrayName.end(), ::isspace), arrayName.end());

                   
                         pointerMap[pointer_string]={arrayName};
                      
                    }
                    else{
                                                                                //case of x=&y
                            pointerMap[pointer_string] = { value_string };
                    }
            }
          
            }


            

          }
   
        }
      }
    // }
    return pointerMap;
}
struct alias_lib : public FunctionPass {
  static char ID;
  alias_lib() : FunctionPass(ID) {}
  

  bool runOnFunction(Function &F) override {

     std::unordered_map<BasicBlock*, std::unordered_map<std::string, std::unordered_set<std::string>>> BlockVarValueMaps;
    std::unordered_map<std::string, std::unordered_set<std::string>>map_before_meet;
    std::unordered_map<std::string, std::unordered_set<std::string>>current_map;
    std::unordered_map<std::string, std::unordered_set<std::string>>new_map;
    std::queue<BasicBlock *> worklist;
   


 //push entry block and then   all other basic blocks into a worklist
   
   
 worklist.push(&F.getEntryBlock());


// Initialize worklist with all basic blocks except the entry block
   for (auto &BB : F) {
     
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
   
         current_map=get_aliases(CurrBlock, current_map,F);
      
          BlockVarValueMaps[CurrBlock]=current_map;
     

        }
       
     
        

        if (CurrBlock->getSinglePredecessor() != nullptr) {
 
            new_map = get_aliases(CurrBlock,current_map, F);


        } else {
            // Multiple predecessors, use meet_and_append
            std::vector<std::unordered_map<std::string, std::unordered_set<std::string>>> pred_maps;
            for (auto *Pred : predecessors(CurrBlock)) {
                pred_maps.push_back(BlockVarValueMaps[Pred]);
         
            }
           
            current_map = meet_and_append(pred_maps, current_map);
           
            new_map=get_aliases(CurrBlock,current_map,F);
           

        }
           BlockVarValueMaps[CurrBlock]=new_map;

        if (!compare_maps(new_map, current_map)) {
            // If there is a change, enqueue successors
            current_map=new_map;
            for (auto *Succ : successors(CurrBlock)) {
                worklist.push(Succ);
            }
        }
       


    }while (!worklist.empty());
   


    exportResultsToTerminal("alias_output.txt", F.getName().str(), new_map);

    return false;
  }
  
}; // end of struct alias_c
}  // end of anonymous namespace

char alias_lib::ID = 0;
static RegisterPass<alias_lib> X("alias_lib", "Alias Analysis Pass for Assignment",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);





