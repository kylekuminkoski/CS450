#include <stdint.h>
#include <string.h>
#include <system.h>

#include <core/io.h>
#include <core/serial.h>
#include "mpx_supt.h"
#include "pcb.h"
#include "time.h"

pcb_queue readyQ; //ready queue
pcb_queue sReadyQ; //suspended ready queue
pcb_queue blockedQ; //blocked queue
pcb_queue sBlockedQ; //suspended blocked queue

pcb_queue *queues[4];

//initiliazes queues
void init(void){
  queues[0] = &readyQ;
  readyQ.head = NULL;
  readyQ.tail = NULL;
  readyQ.length = 0;
  readyQ.sort = PRIORITY;
  
  queues[1] = &sReadyQ;
  sReadyQ.head = NULL;
  sReadyQ.tail = NULL;
  sReadyQ.length = 0;
  sReadyQ.sort = PRIORITY;
  
  queues[2] = &blockedQ;
  blockedQ.head = NULL;
  blockedQ.tail = NULL;
  blockedQ.length = 0;
  blockedQ.sort = FIFO;
  
  queues[3] = &sBlockedQ;
  sBlockedQ.head = NULL;
  sBlockedQ.tail = NULL;
  sBlockedQ.length = 0;
  sBlockedQ.sort = FIFO;
}

//queue based on state
pcb_queue* queueState(processState state){
  if(state == READY){
    return &readyQ;
  }
  else if(state == SUSPENDED_READY){
    return &sReadyQ;
  }
  else if(state == BLOCKED){
    return &blockedQ;
  }
  else{
    return &sBlockedQ;
  }
  
  return NULL;
}
///////////////// HELPER FUNCTIONS //////////////////////////
//find pcb in queue
pcb* find(char *name, pcb_queue *queue){
  int i;
  pcb *curr = queue->head, *rVal = NULL;
  
  for(i = 0; i < queue->length; i++){
     if(strcmp(curr->name, name) == 0){
         rVal = curr;
     }
     else{
     	curr = curr->next; 
	
     }
  }
  
  return rVal;
}

//processes user input class type, invalid input if returns 0
int processType(char* c){
  if(strcmp(c, "APPLICATION") == 0 || strcmp(c, "application") == 0){
    return 1;
  }
  else if(strcmp(c, "A") == 0 || strcmp(c, "a") == 0 ){
    return 1;
  }
  else if(strcmp(c, "SYSTEM") == 0 || strcmp(c, "system") == 0){
    return 2;
  }
  else if(strcmp(c, "S") == 0 || strcmp(c, "s") == 0){
    return 2;
  }
  else{
    return 0;
  }
  
  return 0;
}

////////////////////// FUNCTIONS ////////////////////////////
//allocate memory for new pcb
pcb* allocatePCB(void){
  pcb *new; //pointer to new pcb
  
  //allocate mem
  new = (pcb*)sys_alloc_mem(sizeof(pcb)); //
	
  if(new == NULL){ 
    sprintf("error allocating memory");
    return NULL;
  }
  
  //allocate memory for pcb stack
  new->base = (unsigned char*)sys_alloc_mem(STACK_SIZE); //
	
   if(new->base == NULL){ 
    sprintf("error allocating memory for pcb stack\n");
    sys_free_mem(new);
    return NULL;
  }
  
  //initialize stack top
  new->top = new->base + STACK_SIZE;
  
  return new;
}


//free all memory associated with given PCB
void freePCB(pcb *pcb){ 
  sys_free_mem(pcb->base);
  sys_free_mem(pcb);
}

//call allocatePCB to create empty PCB, initializes the information, sets state to ready
pcb* setupPCB(char *name, char *c, int priority){
  pcb *new; //pointer to new pcb
 
  //allocate new pcb
  new = allocatePCB();
  
  if(new == NULL){
    sprintf("allocation error\n");
    return NULL;
  }
  
  //set values
  strcpy(new->name ,name);
  strcpy(new->classType ,c);
  new->priority = priority;

  new->state = READY;
  
  new->top = new->base + 1024 - sizeof(context); //R3
  
  //initialize stack to null
  memset(new->base, NULL, STACK_SIZE);

  return new;
}

//search all queues for a process with given name
pcb* findPCB(char* name){
  pcb* found;
  int i = 0;
  
  while(i < 4){ //find pcb in queue
   found = find(name, queues[i]); //searches for a specific pcb name in each queue
    
    if(found){
      return found;
    }
    else{
      i++;
    }
  }
  
  sprintf("error - not found in queue\n");
  return NULL;
}


//insert a PCB into appropriate queue
int insertPCB(pcb* insert){
     pcb *curr, *temp;
     insert->next = NULL;
     insert->prev = NULL;


     if(insert->state == BLOCKED){ //BLOCKED QUEUE
       if(blockedQ.head == NULL){ //Q empty
         blockedQ.tail = insert; //only pcb in the queue, set pcb to head and tail
         blockedQ.head = insert;
         insert->next = NULL;
         insert->prev = NULL;
       }
       else{ //insert at tail
         blockedQ.tail->next = insert; //tail next = inserted pcb
         insert->prev = blockedQ.tail; 
         insert->next = NULL; 
         blockedQ.tail = insert; 
       }
       blockedQ.length++;
     }
     else if(insert->state == SUSPENDED_BLOCKED){ //SUSPENDED BLOCKED QUEUE
       if(sBlockedQ.head == NULL){ //Q empty
         sBlockedQ.tail = insert; //only pcb in the queue, set pcb to head and tail
         sBlockedQ.head = insert;
         insert->next = NULL;
         insert->prev = NULL;
       }
       else{ //insert at tail
         sBlockedQ.tail->next = insert;
         insert->prev = sBlockedQ.tail; 
         insert->next = NULL; 
         sBlockedQ.tail = insert; 
       }
       sBlockedQ.length++;
     }
     else if(insert->state == READY){ //READY QUEUE
       if(readyQ.head == NULL && readyQ.tail == NULL){ //Q empty
         readyQ.head = insert; //only pcb in the queue, set pcb to head and tail
         readyQ.tail = insert;
         readyQ.length++;
         return 0;
       }
       else{
         curr = readyQ.head; //current pcb @ head
         temp = NULL;
         
	 //inserts pcb into queue based on priority
         while(curr != NULL && curr->priority >= insert->priority){
           curr = curr->next;
         }
         
         if(curr == NULL){
           if(readyQ.tail == readyQ.head){ 
             temp = readyQ.head;
             temp->next = insert;
             insert->prev = temp;
             readyQ.tail = insert;
             readyQ.length++;
             return 0;
           }
           else{
             readyQ.tail->next = insert;
             insert->prev = readyQ.tail;
             readyQ.tail = insert;
             readyQ.length++;
             return 0;
           }
         }
         if(curr == readyQ.head){ //new head
           if(readyQ.tail == readyQ.head){
             temp = readyQ.head;
             insert->next = temp;
             temp->prev = insert;
             readyQ.head = insert;
             readyQ.length++;
             return 0;
           }
           else{
             insert->next = readyQ.head;
             readyQ.head->prev = insert;
             readyQ.head = insert;
             readyQ.length++;
             return 0;
           }
         }
         else{
           insert->next = curr;
           insert->prev = curr->prev;
           curr->prev->next = insert;
           curr->prev = insert;
           readyQ.length++;
           return 0;
         }
       }
     }
     else if(insert->state == SUSPENDED_READY){ //SUSPENDED READY QUEUE
       if(sReadyQ.head == NULL && sReadyQ.tail == NULL){ //Q empty
         sReadyQ.head = insert; //only pcb in the queue, set pcb to head and tail
         sReadyQ.tail = insert;
         sReadyQ.length++;
         return 0;
       }
       else{
         curr = sReadyQ.head;
         temp = NULL;
         
         while(curr != NULL && curr->priority >= insert->priority){
           curr = curr->next;
         }
         
         if(curr == NULL){ 
           if(sReadyQ.tail == sReadyQ.head){
             temp = sReadyQ.head;
             temp->next = insert;
             insert->prev = temp;
             sReadyQ.tail = insert;
             sReadyQ.length++;
             return 0;
           }
           else{
             sReadyQ.tail->next = insert;
             insert->prev = sReadyQ.tail;
             sReadyQ.tail = insert;
             sReadyQ.length++;
             return 0;
           }
         }
         if(curr == readyQ.head){ //new head
           if(sReadyQ.tail == sReadyQ.head){
             temp = sReadyQ.head;
             insert->next = temp;
             temp->prev = insert;
             sReadyQ.head = insert;
             sReadyQ.length++;
             return 0;
           }
           else{
             insert->next = sReadyQ.head;
             sReadyQ.head->prev = insert;
             sReadyQ.head = insert;
             sReadyQ.length++;
             return 0;
           }
         }
         else{
           insert->next = curr;
           insert->prev = curr->prev;
           curr->prev->next = insert;
           curr->prev = insert;
           sReadyQ.length++;
           return 0;
         }
       }
     } 
     
   sprintf("error");  
   return 0;  
}

//remove a PCB from the queue in which it is being currently stored
int removePCB(pcb* remove){	
  pcb_queue *queue = queueState(remove->state); //queue to be removed from
  
  if(remove == queue->head && remove == queue->tail){ //only pcb in queue
    queue->head = NULL;
    queue->tail = NULL;
    queue->length--;
  }
  else if(remove == queue->head){ //pcb is head
    queue->head = queue->head->next; //set next to the next pcb's next
    queue->head->prev = NULL; //prev = null
    queue->length--; 
  }
  else if(remove == queue->tail){ //pcb is tail
    queue->tail = queue->tail->prev; //set prev to the previous pcb's prev
    queue->tail->next = NULL; //next = null
    queue->length--;
  }
  else{ //pcb in middle of queue
    remove->prev->next = remove->next; 
    remove->next->prev = remove->prev;
    queue->length--;
  }
  
  return 0;
}


////////////////////// COMMANDS ////////////////////////////
int suspendPCB(char* name){
  if(!findPCB(name)){ //name must be valid
    sprintf("There is no PCB under this name.");
    return 0;
  }
  
  pcb* pcb = findPCB(name); //obtain pcb
  
  if(pcb->state == READY){ //READY Q
    removePCB(pcb); //remove from queue
    pcb->state = SUSPENDED_READY; //change state
    sprintf("sucessfully set state to suspended ready\n");
    insertPCB(pcb); //insert into appropriate queue
    return 0; 
  }
  else if(pcb->state == BLOCKED){ //BLOCKED Q
    removePCB(pcb); 
    pcb->state = SUSPENDED_BLOCKED; 
    sprintf("successfully set state to suspended blocked\n");
    insertPCB(pcb); 
    return 0;  
  }   

  sprintf("error suspending pcb"); //error
  return 0;
}

int blockPCB(char* name){
  if(!findPCB(name)){ //name must be valid
    sprintf("There is no PCB under this name.");
    return 0;
  }
  
  pcb* pcb = findPCB(name); //obtain pcb
  
  if(pcb->state == READY){ //READY Q
    removePCB(pcb); //remove from queue
    pcb->state = BLOCKED; //change state
    sprintf("successfully set state to blocked\n");
    insertPCB(pcb); //insert into appropriate queue
    return 1;
  }
  else if(pcb->state == SUSPENDED_READY){ //SUSPENDED READY Q
    removePCB(pcb); 
    pcb->state = SUSPENDED_BLOCKED; 
    sprintf("successfully set state to suspended blocked\n");
    insertPCB(pcb);
    return 1;
  }
  
  sprintf("error blocking pcb\n"); //error
  return 0;
}

int createPCB(char* name, char *c, int priority){
  pcb *create; 
  int temp;
  
  if(priority > 9 && priority < 0){ //check priority between 0-9
    sprintf("priority must be between 0 and 9\n");
    return 0;
  }
  
  //determine process type with helper function
  temp = processType(c);
  
  if(temp == 1){
    c = "APPLICATION";
  }
  else if(temp == 2){
    c = "SYSTEM";
  }
  else {
    sprintf("error - invalid class type\n");
    return 0;
  }
  
  create = setupPCB(name, c, priority); //create pcb with setuppcb
  
  create->next = NULL;
  create->prev = NULL;
  
  insertPCB(create); //insert into correct queue
  sprintf("successfully inserted into a queue\n"); //success
 
  return 0;
}

int deletePCB(char* name){
  if(!findPCB(name)){ //pcb not valid
    sprintf("error - pcb not found\n");
    return 0;
  }

   pcb* pcb = findPCB(name); //obtain pcb
   removePCB(pcb); //remove from queue
   sprintf("successfully removed from queue\n"); //sucess 
   freePCB(pcb); //free pcb
   return 0;
}

void setPriority(char*name, int priority){
  if(!findPCB(name)){ //pcb not valid
    sprintf("There is no PCB under this name.");
    return;
  }

  pcb* pcb = findPCB(name); //obtain pcb

  pcb->priority = priority; //set priority
}

int resumePCB(char* name){
  if(!findPCB(name)){ //pcb not valid
    sprintf("There is no PCB under this name.");
    return 0;
  }
  
  pcb* pcb = findPCB(name); //obtain pcb

  if(pcb->state == SUSPENDED_READY){ //SUSPENDED READY Q
    removePCB(pcb); //remove from queue
    pcb->state = READY; //change state
    sprintf("successfully set state to ready\n");
    insertPCB(pcb); //insert into appropriate queue
    return 0;
  }
  else if(pcb->state == SUSPENDED_BLOCKED){ //SUSPENDED BLOCKED Q
    removePCB(pcb); 
    pcb->state = BLOCKED; 
    sprintf("successfully set state to blocked\n");
    insertPCB(pcb); 
    return 0;
  }

  sprintf("error resuming pcb\n"); //error
  return 0;
}


int unblockPCB(char* name){
  if(!findPCB(name)){ //name must be valid
    sprintf("There is no PCB under this name.");
    return 0;
  }

  pcb* pcb = findPCB(name); //obtain pcb

  if(pcb->state == BLOCKED){ //BLOCKED Q
    removePCB(pcb); //remove from queue
    pcb->state = READY; //change state
    sprintf("successfully set state to ready\n");
    insertPCB(pcb); //insert into appropraite queue
    return 0;
  }
  else if(pcb->state == SUSPENDED_BLOCKED){ //SUSPENDED BLOCKED Q
    removePCB(pcb); 
    pcb->state = SUSPENDED_READY; 
    sprintf("successfully set state to suspended ready\n");
    insertPCB(pcb); 
    return 0;
  }

  sprintf("error unblocking pcb\n"); //error
  return 0;
}

void showPCB(char* name) {
  char *pc, *priority; //char pointer
  char pt[32]; //char array (itoa function)

  if(!findPCB(name)){ //pcb not valid
    sprintf("There is no PCB under this name.");
    return;
  }

  pcb* pcb = findPCB(name); //obtain pcb
  
  if(pcb == NULL){
    sprintf("error - PCB null");
    return;
  }
	
  pc = pcb->classType; //pc = class type
  priority = itoa(pcb->priority, pt, 10); //obtain priority w/ itoa

  sprintf("\nName:          ");
  sprintf(name);
  sprintf("\n");
  sprintf("Process Class: ");
  sprintf(pc);
  sprintf("\n");
  sprintf("State:         ");
  //obtain state
  if(pcb->state == READY)
    sprintf("READY");
  else if(pcb->state == SUSPENDED_READY)
    sprintf("SUSPENDED_READY");
  else if(pcb->state == BLOCKED)
    sprintf("BLOCKED");
  else if(pcb->state == SUSPENDED_BLOCKED)
    sprintf("SUSPENDED_BLOCKED");
  sprintf("\n");
  sprintf("Priority:      ");
  sprintf(priority);
  sprintf("\n");
}

void showAll(){
  char pt[32]; //char array (itoa function)
  pcb *node;

  //print pcb's only from specific queue	
  node = queues[0]->head; //READY Q

  if(node != NULL){
    sprintf("Here are the elements in the ready queue:\n");
    while(node != NULL){
      char *n = node->name;
      char *pc = node->classType;
      char *priority = itoa(node->priority, pt, 10);

      sprintf("\nName:            ");
      sprintf(n);
      sprintf("\n");
      sprintf("Process Class:   ");
      sprintf(pc);
      sprintf("\n");
      sprintf("Priority:        ");
      sprintf(priority);
      sprintf("\n");

      node = node->next; //next node in queue
    }
  }
  else{
    sprintf("There are no elements in the ready queue\n");
  }

  node = queues[1]->head; //SUSPENDED READY Q

  if(node==NULL){
    sprintf("There are no elements in the suspended ready queue\n");
  }
  else{
    sprintf("Here are the elements in the suspended ready queue:\n");
    while(node != NULL){
      char *n = node->name;
      char *pc = node->classType;
      char *priority = itoa(node->priority, pt, 10);

      sprintf("\nName:            ");
      sprintf(n);
      sprintf("\n");
      sprintf("Process Class:   ");
      sprintf(pc);
      sprintf("\n");
      sprintf("Priority:        ");
      sprintf(priority);
      sprintf("\n");

      node = node->next; //next node in queue
    }
  } 

  node = queues[2]->head; //BLOCKED Q

  if(node==NULL){
    sprintf("There are no elements in the blocked queue\n");
  }
  else{
    sprintf("Here are the elements in the blocked queue:\n");
    while(node != NULL){
      char *n = node->name;
      char *pc = node->classType;
      char *priority = itoa(node->priority, pt, 10);

      sprintf("\nName:            ");
      sprintf(n);
      sprintf("\n");
      sprintf("Process Class:   ");
      sprintf(pc);
      sprintf("\n");
      sprintf("Priority:        ");
      sprintf(priority);
      sprintf("\n");

    node = node->next; //next node in queue
    }
  } 

  node = queues[3]->head; //SUSPENDED BLOCKED Q

  if(node==NULL){
    sprintf("There are no elements in the suspended blocked queue\n");
  }
  else{
    sprintf("Here are the elements in the suspended blocked queue:\n");
    while(node != NULL){
      char *n = node->name;
      char *pc = node->classType;
      char *priority = itoa(node->priority, pt, 10);

      sprintf("\nName:            ");
      sprintf(n);
      sprintf("\n");
      sprintf("Process Class:   ");
      sprintf(pc);
      sprintf("\n");
      sprintf("Priority:        ");
      sprintf(priority);
      sprintf("\n");

    node = node->next; //next node in queue
    }
  } 
}

void showReady(){
  char *n, *pc, *priority;
  char pt[15];

  pcb* node = queues[0]->head; //READY Q

  if(node!=NULL){
    sprintf("Here are the elements in the ready queue:\n");
    while(node != NULL){
      n = node->name;
      pc = node->classType;
      priority = itoa(node->priority, pt, 10);

      sprintf("\nName:            ");
      sprintf(n);
      sprintf("\n");
      sprintf("Process Class:   ");
      sprintf(pc);
      sprintf("\n");
      sprintf("Priority:        ");
      sprintf(priority);
      sprintf("\n");

      node = node->next; //next node in queue
    }
  }
  else{
    sprintf("There are no elements in the ready queue\n");
  }

  node = queues[1]->head; //SUSPENDED READY Q

  if(node==NULL){
    sprintf("There are no elements in the suspended ready queue\n");
  }
  else{
    sprintf("Here are the elements in the suspended ready queue:\n");
    while(node != NULL){
      n = node->name;
      pc = node->classType;
      priority = itoa(node->priority, pt, 10);

      sprintf("\nName:            ");
      sprintf(n);
      sprintf("\n");
      sprintf("Process Class:   ");
      sprintf(pc);
      sprintf("\n");
      sprintf("Priority:        ");
      sprintf(priority);
      sprintf("\n");

      node = node->next; //next node in queue
    }
  } 

}

void showBlocked(){
  char *n, *pc, *priority;
  char pt[15];

  pcb* node = queues[2]->head; //BLOCKED Q

  if(node!=NULL){
    sprintf("Here are the elements in the blocked queue:\n");
    while(node != NULL){

      n = node->name;
      pc = node->classType;
      priority = itoa(node->priority, pt, 10);

      sprintf("\nName:            ");
      sprintf(n);
      sprintf("\n");
      sprintf("Process Class:   ");
      sprintf(pc);
      sprintf("\n");
      sprintf("Priority:        ");
      sprintf(priority);
      sprintf("\n");

      node = node->next; //next node in queue
    }
  }
  else{
    sprintf("There are no elements in the bocked queue\n");
  }

  node = queues[3]->head; //SUSPENDED BLOCKED Q

  if(node==NULL){
    sprintf("There are no elements in the suspended blocked queue\n");
  }
  else{
    sprintf("Here are the elements in the suspended blocked queue:\n");
    while(node != NULL){
      n = node->name;
      pc = node->classType;
      priority = itoa(node->priority, pt, 10);

      sprintf("\nName:            ");
      sprintf(n);
      sprintf("\n");
      sprintf("Process Class:   ");
      sprintf(pc);
      sprintf("\n");
      sprintf("Priority:        ");
      sprintf(priority);
      sprintf("\n");

      node = node->next; //next node in queue
    }
  } 

}
