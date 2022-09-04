#include "mpx_supt.h"
#include "pcb.h"
#include "procsr3.h"

extern param params;
pcb *cop; //cop - currently operating process
context *gcontext;
u32int* sys_call(context *registers);
void yield();
void loadr3();
void makeProc1();
void makeProc2();
void makeProc3();
void makeProc4();
void makeProc5();
pcb* nextProc();



