#include "mpx_supt.h"

#define STACK_SIZE 1024

//state types
typedef enum{
  READY,
  BLOCKED,
  SUSPENDED_READY,
  SUSPENDED_BLOCKED,
  RUNNING
}processState;

//PCB
typedef struct pcb{
  char name[30], classType[20];
  int priority;
  processState state;
  //pcb
  struct pcb *next, *prev;
  //stack
  unsigned char stack[STACK_SIZE];
  unsigned char *top, *base;
}pcb;

//sort order
typedef enum{
  FIFO,
  PRIORITY
}sortOrder;

//pcb queue
typedef struct{
  pcb *head, *tail;
  int length; 
  sortOrder sort;
}pcb_queue;
  
extern pcb_queue *queues[]; //extends visibility

extern pcb_queue readyQ; //ready queue
extern pcb_queue sReadyQ; //suspended ready queue
extern pcb_queue blockedQ; //blocked queue
extern pcb_queue sBlockedQ; //suspended blocked queue

//R3
typedef struct context {
  u32int gs, fs, es, ds; //data segment registers
  u32int edi, esi, ebp, esp, ebx, edx, ecx, eax; //32-bit general purpose registers
  u32int eip, cs, eflags; //instruction pointer //code segment //32 bit flags registers
}context;



//functions
void init(void);
pcb_queue* queueState(processState state);
pcb* allocatePCB(void);
void freePCB(pcb *pcb);
pcb* setupPCB(char *name, char *c, int priority);
pcb* findPCB(char* name);
int insertPCB(pcb* insert);
int removePCB(pcb* remove);
//help functions
pcb* find(char *name, pcb_queue *queue);
int processType(char* c);
//commands
int suspendPCB(char* name);
int blockPCB(char* name);
int createPCB(char* name, char *c, int priority);
int deletePCB(char* name);
int resumePCB(char* name);
int unblockPCB(char* name);
void showPCB(char* name);
void showAll();
void showReady();
void showBlocked();
void setPriority();
