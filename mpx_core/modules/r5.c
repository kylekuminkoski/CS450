#include <stdint.h>
#include <string.h>
#include <system.h>
#include "r5.h"
#include "mpx_supt.h" //sprintf
#include <mem/heap.h> //kmalloc
#include <core/serial.h>
#include "time.h" //itoa

list allocatedList;
list freeList;
u32int memoryStart;
char s[32], b[32];

//init heap
void initHeap(u32int size){
  //using kmalloc allocate bytes + size of cmcb and lmcb
  memoryStart = kmalloc(size + sizeof(cmcb));

  //declare a new cmbc
  cmcb* initCMBC = (cmcb*) memoryStart;
  
  //set the contents of the cmbc
  initCMBC->next = NULL;
  initCMBC->prev = NULL;
  initCMBC->size = size;
  initCMBC->allocated = 0;
  initCMBC->beginA = memoryStart + sizeof(cmcb);

  allocatedList.head = NULL;
  allocatedList.head->next = NULL;
  allocatedList.head->prev = NULL;

  freeList.head = initCMBC;
}

  u32int allocate(u32int size){
    //create a variable that represents the total bytes needed
    u32int insertSize = size + sizeof(cmcb);

    //declare a new cmbc and set it equal to the head
    cmcb* curr = freeList.head;
         
    while(curr != NULL){
      //if it finds a block big enough to store it
      if(curr->size >= insertSize){
        u32int begin = curr->beginA;
        
        //adjust the beginning address of the free block
        cmcb* newFree = (cmcb*) (curr->beginA + size);

        //set the contents of the cmbc
        newFree->prev = NULL;
        newFree->next = NULL;
        newFree->size = curr->size - insertSize;
        newFree->allocated = 0;
        newFree->beginA = curr->beginA + insertSize;

        //remove from free list
        removeCMCB(curr);

        //insert the new free
        insertCMCB(newFree);

        curr->allocated=1;
        curr->prev=NULL;
        curr->next=NULL;
        curr->size = size;
        curr->beginA = begin;
        insertCMCB(curr);


        return curr->beginA;
      }

      //this moves the next free block
      curr = curr->next;
    }

    sprintf("\nThere is no free block with enough memory to allocate specified size\n");
    return 0;
  }

int freeMemory(void* bA){
  //get the head of the allocated list
  cmcb* currentAllocatedCMCB = allocatedList.head;

  while(currentAllocatedCMCB != NULL){
    //if there is an address match remove the cmcb
    if(currentAllocatedCMCB->beginA == (u32int) atoi(bA)){
      cmcb* newFree = currentAllocatedCMCB;

      removeCMCB(currentAllocatedCMCB);

      newFree->allocated=0;
      newFree->next=NULL;
      newFree->prev=NULL;


      insertCMCB(newFree);

      return 1;
    }
      currentAllocatedCMCB = currentAllocatedCMCB->next;
  }
    
  sprintf("\nThere is no memory to allocate at this address.\n");
  return 0;
}

void showAllocated(){
  
  if(allocatedList.head == NULL){
    sprintf("no allocated memory\n");
  }
  else{
    cmcb* temp = allocatedList.head;
    
    while(temp != NULL){
      sprintf("size:  ");
      sprintf(itoa(temp->size, s, 10));
      sprintf("\n");
      sprintf("beginning address: ");
      sprintf(itoa(temp->beginA, b, 10));
      sprintf("\n\n");
      temp = temp->next;
    }
  }
}

void showFree(){
  
  if(freeList.head == NULL){
    sprintf("no free memory\n");
  }
  else{
    cmcb* temp = freeList.head;
    
    while(temp != NULL){
      sprintf("size:  ");
      sprintf(itoa(temp->size, s, 10));
      sprintf("\n");
      sprintf("beginning address: ");
      sprintf(itoa(temp->beginA, b, 10));
      sprintf("\n\n");
      temp = temp->next;
    }
  }
}

int isEmpty(){
  if(allocatedList.head == NULL){
    return 1;
  }
  else{
    return 0;
  }
}

int removeCMCB(cmcb* remove){
  //free list
  if(remove->allocated == 0){
    if(remove == freeList.head){ 
      freeList.head = remove->next;
      freeList.head->prev = NULL;
      return 1;
    }
    else{
      remove->prev->next = remove->next;
      remove->next->prev = remove->prev;
      return 1;
    }
    return 0;
  }
  //allocated list
  else{
    if(remove == allocatedList.head){
      remove = remove->next;
      remove->prev = NULL;
      return 1;
    }
    else{
      remove->prev->next = remove->next;
      remove->next->prev = remove->prev;
      return 1;
    }
    return 0;
  }
  return 0;

}

int insertCMCB(cmcb* insert){
  if(insert->allocated==0){
    cmcb* curr = freeList.head;
    if(curr==NULL){
      insert->prev=NULL;
      insert->next=NULL;
      freeList.head=insert;
      return 1;
    }
    while(curr!=NULL){
        if(curr->beginA < insert->beginA){
          //insert before curr
          cmcb* temp = curr;
          temp->prev = insert;
          insert->prev = curr->prev;
          insert->next = temp;
          curr= insert;
          return 1;
        }
        else{
          cmcb* temp = curr->next;
          temp->prev = insert;
          insert->prev= curr;
          insert->next= temp;
          curr->next = insert;

        //now check if there are adjacent free blocks
        //if the next free cmcb address and see if its equal to current beginA + size + cmcb
        if((insert->beginA + insert->size + sizeof(cmcb)) == insert->next->beginA){
          insert->prev->size += sizeof(cmcb) + insert->size;
          removeCMCB(insert);
        }
        else if(curr->beginA == (insert->beginA + insert->size + sizeof(cmcb))){
          curr->size += sizeof(cmcb) + insert->size;
          removeCMCB(insert);
        }
        //if the prev free cmcb address and see if its 
        
          return 1;
        }
        
        curr = curr->next;
      return 1;
    }
    return 0;
  }
  else{
    cmcb* curr = allocatedList.head;
    if(curr==NULL){
      insert->prev=NULL;
      insert->next=NULL;
      allocatedList.head=insert;
      return 1;
    }
    while(curr!=NULL){
      if(curr->beginA > insert->beginA){
        //insert before curr
        cmcb* temp = curr;
        temp->prev = insert;
        insert->prev = curr->prev;
        insert->next = temp;
        curr= insert;
        return 1;
      }
      else{
        cmcb* temp = curr->next;
        temp->prev = insert;
        insert->prev= curr;
        insert->next= temp;
        curr->next = insert;
        return 1;
      }
      curr=curr->next;
    }
    return 0;
  }
  return 0;
}
