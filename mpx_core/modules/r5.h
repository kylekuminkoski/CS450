#include <stdint.h>
#include <string.h>
#include <system.h>
#include "mem/heap.h"

typedef struct cmcb cmcb;

struct cmcb{
  int allocated;
  u32int size, beginA;
  cmcb *next, *prev;
};

typedef struct list{
  cmcb *head;
}list;

extern list freeList, allocatedList;


//Commands
void showFree();
void showAllocated();
//Temp 
void initHeap();
u32int allocate(u32int size);
int freeMemory(void* bA);
int removeCMCB(cmcb* remove);
int insertCMCB(cmcb* insert);
int isEmpty();



