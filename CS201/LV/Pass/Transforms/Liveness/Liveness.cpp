#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/CFG.h"
#include <string>
#include <map>
#include <iterator>
#include <set>
#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>

using namespace llvm;
using namespace std;

#define DEBUG_TYPE "Liveness"

using namespace llvm;

namespace {
struct Liveness : public FunctionPass {
  string func_name = "test";
  static char ID;
  Liveness() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
   // errs() << "Liveness: ";
   // errs() << F.getName() << "\n";
      if (F.getName() != func_name) return false;
      string file_name(F.getParent()->getSourceFileName());
      if(file_name == "Test.c"){
          file_name = "1.out";
      }else if(file_name == "Test2.c"){
          file_name  = "2.out";
      }else{
          file_name.replace(file_name.find("c"), string("c").length(), "out");
      }
      string output_string;
      std::map <int, BasicBlock*> BBOrder;
      std::map <StringRef, set<string>*> KILL;
      std::map <StringRef, set<string>*> UPEXP;
      std::map <StringRef, set<string>*> LIVEOUT;
      
        for (auto& basic_block : F)
        {
            StringRef bbV = basic_block.getName();
            
            int BlockID = BBOrder.size();
            BBOrder[BlockID + 1] = &basic_block;
            
            auto VarKill = new set<string>;
            auto UEVar = new set<string>;
            auto LiveOut = new set<string>;

            for (auto& inst : basic_block)
            {
                
                // Calculate Uev
              if(inst.getOpcode() == Instruction::Load){
                  if(LoadInst *load = dyn_cast<LoadInst>(&inst)){
                      StringRef UEV_Value_name = load->getOperand(0)->getName();
                    if(VarKill->find(UEV_Value_name) == VarKill->end()){
                        if(UEVar->find(UEV_Value_name) == UEVar->end()){
                            UEVar->insert(UEV_Value_name);
                        }
                    }
                  }
              }
                  
                //Calculate Kill
              if(inst.getOpcode() == Instruction::Store){
                  if(StoreInst *store = dyn_cast<StoreInst>(&inst)){
                      StringRef Kill_Value_name = store->getOperand(1)->getName();
                      if(VarKill->find(Kill_Value_name) == VarKill->end()){
                          VarKill->insert(Kill_Value_name);
                          
                      }
                  }
              }

                KILL[bbV] = VarKill;
                UPEXP[bbV] = UEVar;
                LIVEOUT[bbV] = LiveOut;
                
            }
        }
      
      
      // Compute Liveout
      bool check_change = true;
      
      do{
          check_change = false;
          for (auto it0 = BBOrder.size(); it0 > 0; --it0){
            auto current_bb = BBOrder[it0];
            auto Live_Current = LIVEOUT[current_bb->getName()];
            for (BasicBlock *Succ : successors(current_bb)){
                set<string> result_difference;
                set<string> result_union;
                auto temp_Live_Current = new set<string>;
                auto Live_Succ = LIVEOUT[Succ->getName()];
                auto Kill_Succ = KILL[Succ->getName()];
                auto UEV_Succ = UPEXP[Succ->getName()];
                
                //difference
                for (auto iter = Live_Succ->begin(); iter != Live_Succ->end(); ++iter){
                    if(Kill_Succ->find(*iter) == Kill_Succ->end()){
                        result_difference.insert(*iter);
                    }
                }
              
                //union
                result_union = result_difference;
              
                for (auto iter2 = UEV_Succ->begin(); iter2 != UEV_Succ->end(); ++iter2){
                    if(result_difference.find(*iter2) == result_difference.end()){
                        result_union.insert(*iter2);
                    }
                }
              //curren bb union
                for (auto iter3 = result_union.begin(); iter3 != result_union.end(); ++iter3){
                    if(temp_Live_Current->find(*iter3) == temp_Live_Current->end()){
                        temp_Live_Current->insert(*iter3);
                    }
                }
              
                for (auto iter4 = temp_Live_Current->begin(); iter4 != temp_Live_Current->end(); ++iter4){
                    if(Live_Current->find(*iter4) == Live_Current->end()){
                        Live_Current->insert(*iter4);
                        check_change = true;
                    }
                }
            }
          }
      } while (check_change);
      
      auto Block_ID = BBOrder.begin();
      while(Block_ID != BBOrder.end()){
          string temp_string;
          string Block_Name = Block_ID->second->getName();
          errs() << "----- " << Block_Name << " -----\n\n" << "UEVAR: ";
          temp_string = "----- " + Block_Name + " -----\n\n" + "UEVAR: ";
          output_string.append(temp_string);
          for (auto UEVAR_print = UPEXP[Block_Name]->begin(); UEVAR_print != UPEXP[Block_Name]->end(); ++UEVAR_print){
              errs() << *UEVAR_print << " ";
              temp_string = *UEVAR_print + " ";
              output_string.append(temp_string);
          }
          errs() << "\n\nVARKILL: ";
          temp_string = "\n\nVARKILL: ";
          output_string.append(temp_string);
          for (auto VARKILL_print = KILL[Block_Name]->begin(); VARKILL_print != KILL[Block_Name]->end(); ++VARKILL_print){
              errs() << *VARKILL_print << " ";
              temp_string = *VARKILL_print + " ";
              output_string.append(temp_string);
          }
          errs() << "\n\nLIVEOUT: ";
          temp_string = "\n\nLIVEOUT: ";
          output_string.append(temp_string);
          for (auto LIVEOUT_print = LIVEOUT[Block_Name]->begin(); LIVEOUT_print != LIVEOUT[Block_Name]->end(); ++LIVEOUT_print){
              errs() << *LIVEOUT_print << " ";
              temp_string = *LIVEOUT_print + " ";
              output_string.append(temp_string);
          }
          errs() << "\n\n";
          temp_string = "\n\n";
          output_string.append(temp_string);
          ++Block_ID;
      }
      ofstream output;
      output.open(file_name);
      output << output_string;
      output.close();
    return false;
  }
}; // end of struct Liveness
}  // end of anonymous namespace

char Liveness::ID = 0;
static RegisterPass<Liveness> X("Liveness", "Liveness Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
