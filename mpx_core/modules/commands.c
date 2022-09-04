#include <stdint.h>
#include <string.h>
#include <system.h>

#include <core/io.h>
#include <core/serial.h>
#include "mpx_supt.h"


void version(){
sprintf("Current Version: R6\nCompletion Date: 12/9/2021\n");
}

void help(){

sprintf("Menu of available commands:\n");
//r1
sprintf(" 1) Get Date - Returns the date \n 2) Set Date - Sets value for date \n 3) Get Time - Returns value of time in MS \n 4) Set Time - Set time in MS \n 5) Version - Returns the current version of Happy's OS \n");
sprintf(" 6) Help - Returns list of available functions \n 7) Shutdown - Closes Happy's OS \n 8) Clear Page - Clears the page \n");
//r2
sprintf(" 9) Suspend - Sets PCB in suspended state \n 10) Resume - Removes a PCB from the suspended state\n");
sprintf(" 11) Set Priority - Sets PCB's priority \n 12) Show PCB - Displays PCB information \n 13) Show all processes - Displays all PCB information in both ready and blocked states \n 14) Show ready processes - Displays all PCB information in the ready state\n 15) Show blocked processes - Displays all PCB information in the blocked state \n");
//r3/r4
sprintf(" 16) Loadr3 - loads all r3 processes into memory\n 17) Alarm - prints a message at a user inputted time\n");
//r5
sprintf(" 18) Show Allocated - shows address and size of block in the allocated list\n 19) Show Free - shows address and size of block in the free list\n");
}

int shutdown(){
char cmdBuffer[100];
int bufferSize;
	
	
sprintf("\x1b[1;92m");
sprintf("Are you sure [y or n]: ");
sprintf("\x1b[1;37m");

memset(&bufferSize,'\0',100);
bufferSize = 99;
sys_req(READ, COM1,cmdBuffer,&bufferSize);


if (strcmp(cmdBuffer,"y") == 0){
return 1;
}

return 0;
}

  
 int clear(){
  
 sprintf("\x1b[2J");
 sprintf("\x1b[H");
 return 0;
  
}
