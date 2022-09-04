#include "mpx_supt.h"
#include "r3.h"

#include <stdint.h>
#include <string.h>
#include <system.h>

#include <core/io.h>
#include <core/serial.h>


//system call
u32int* sys_call(context *registers){
   //check if sys_call has been called before
  if (cop == NULL){ //not been called 
    gcontext = registers; //save reference to old context
  }
  else{ //has been called
    if(params.op_code == IDLE){ //save the context (reassign cop's stack top)
      cop->top = (unsigned char *) registers;
      
      //removes and reinserts into correct queue
      removePCB(cop); 
      insertPCB(cop);
    }
    else if(params.op_code == EXIT){ //free cop
      deletePCB(cop->name);
    }
  }
  
  cop = nextProc(); //get next process
  
  if(cop == NULL){
    return (u32int *) gcontext; //return context saved previously
  }
  
  return (u32int *) cop->top; //return stack top
}

//yield - commhand yields to other processes
void yield(){
  asm volatile ("int $60"); 
}

//loadr3 - loads all r3 processes into suspended ready
void loadr3(){
  makeProc1();
  makeProc2();
  makeProc3();
  makeProc4();
  makeProc5();
}

//Processes 
void makeProc1(){
  createPCB("r3process1", "SYSTEM", 1);
  pcb* new = findPCB("r3process1");
  context *cp = (context*) (new->top);
  memset(cp, 0, sizeof(context));
  cp->fs = 0x10;
  cp->gs = 0x10;
  cp->ds = 0x10;
  cp->es = 0x10;
  cp->cs = 0x8;
  cp->ebp = (u32int)(new->stack);
  cp->esp = (u32int)(new->top);
  cp->eip = (u32int) proc1;
  cp->eflags = 0x202;
  suspendPCB("r3process1");
}

void makeProc2(){
  createPCB("r3process2", "APPLICATION", 1);
  pcb* new = findPCB("r3process2");
  context *cp = (context*) (new->top);
  memset(cp, 0, sizeof(context));
  cp->fs = 0x10;
  cp->gs = 0x10;
  cp->ds = 0x10;
  cp->es = 0x10;
  cp->cs = 0x8;
  cp->ebp = (u32int)(new->stack);
  cp->esp = (u32int)(new->top);
  cp->eip = (u32int) proc2;
  cp->eflags = 0x202;
  suspendPCB("r3process2");
}

void makeProc3(){
  createPCB("r3process3", "SYSTEM", 1);
  pcb* new = findPCB("r3process3");
  context *cp = (context*) (new->top);
  memset(cp, 0, sizeof(context));
  cp->fs = 0x10;
  cp->gs = 0x10;
  cp->ds = 0x10;
  cp->es = 0x10;
  cp->cs = 0x8;
  cp->ebp = (u32int)(new->stack);
  cp->esp = (u32int)(new->top);
  cp->eip = (u32int) proc3;
  cp->eflags = 0x202;
  suspendPCB("r3process3");
}

void makeProc4(){
  createPCB("r3process4", "APPLICATION", 1);
  pcb* new = findPCB("r3process4");
  context *cp = (context*) (new->top);
  memset(cp, 0, sizeof(context));
  cp->fs = 0x10;
  cp->gs = 0x10;
  cp->ds = 0x10;
  cp->es = 0x10;
  cp->cs = 0x8;
  cp->ebp = (u32int)(new->stack);
  cp->esp = (u32int)(new->top);
  cp->eip = (u32int) proc4;
  cp->eflags = 0x202;
  suspendPCB("r3process4");
}

void makeProc5(){
  createPCB("r3process5", "APPLICATION", 1);
  pcb* new = findPCB("r3process5");
  context *cp = (context*) (new->top);
  memset(cp, 0, sizeof(context));
  cp->fs = 0x10;
  cp->gs = 0x10;
  cp->ds = 0x10;
  cp->es = 0x10;
  cp->cs = 0x8;
  cp->ebp = (u32int)(new->stack);
  cp->esp = (u32int)(new->top);
  cp->eip = (u32int) proc5;
  cp->eflags = 0x202;
  suspendPCB("r3process5");
}


pcb* nextProc(){
  pcb* runNext = readyQ.head; 
  while(runNext->state == SUSPENDED_READY && runNext != NULL){
    runNext = runNext->next;
  }
  
  removePCB(runNext);
  //set runNext state to running
  
  return runNext;
}
