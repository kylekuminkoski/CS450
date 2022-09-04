/*
  ----- cmd_hand.c -----

  Description..: Contains command handler
  
*/

#include <stdint.h>
#include <system.h>
#include <string.h>
#include <core/io.h>
#include <core/serial.h>

#include "mpx_supt.h"
#include "time.h"
#include "commands.h"
#include "r3.h"
//#include "R4.h"
#include "r5.h"


void comhand() {

  char cmdBuffer[100];
  int bufferSize;
  //int quit = 0;
  char extBuffer[20];
  int extSize;

/////////////////////////////////////////////////////////////////////////////////////////
sprintf("Booting The Happy 21 OS...\n");
sprintf("\x1b[5m");//blinking on
sprintf("\x1b[1;33m"); //bright yellow foreground
sprintf("\x1b[1;100m"); //black background
sprintf("---------------------------------------------------------------------------------\n"); //70 dashes
sprintf("-----------------------------   --------------------   --------------------------\n");
sprintf("-----------------------------   --------------------   --------------------------\n");
sprintf("-----------------------------   --------------------   --------------------------\n");
sprintf("-----------------------------   --------------------   --------------------------\n");
sprintf("-----------------------------   --------------------   --------------------------\n");
sprintf("-----------------------------   --------------------   --------------------------\n");
sprintf("-----------------------------   --------------------   --------------------------\n");
sprintf("-----------------------------   --------------------   --------------------------\n");
sprintf("-----------------------------   --------------------   --------------------------\n");
sprintf("-----------------------------   --------------------   --------------------------\n");
sprintf("-------   -------------------   --------------------   ------------------   -----\n");
sprintf("--------   -------------------------------------------------------------   ------\n");
sprintf("-----------   -------------------------------------------------------   ---------\n");
sprintf("---------------   -----------------------------------------------   -------------\n");
sprintf("------------------                                                ---------------\n");
sprintf("---------------------------------------------------------------------------------\n");
sprintf("---------------------------------------------------------------------------------\n");
sprintf("\x1b[1;49m");  // default background set
sprintf("\x1b[25m"); //blinking off
sprintf("\x1b[1;37m"); //foreground set to white
sprintf("\n");
sprintf("Welcome to Happy's Operating System!\n");
sprintf("To use a command, enter the number next to the command you would like to use.\n");
/////////////////////////////////////////////////////////////////////////////////////////
//r1
sprintf(" 1) Get Date\n 2) Set Date\n 3) Get Time\n 4) Set Time\n 5) Version\n 6) Help\n 7) Shutdown\n 8) Clear Page\n");
//r2
sprintf(" 9) Suspend\n 10) Resume\n 11) Set Priority\n 12) Show PCB\n 13) Show all Processes\n 14) Show Ready Processes\n 15) Show Blocked Processes\n");
//r3/r4
sprintf(" 16) Loadr3\n 17) Alarm\n");
//r5
//keep show free and show allocated. delete the rest
sprintf(" 18) Show Free\n 19) Show Allocated\n");
/////////////////////////////////////////////////////////////////////////////////////////


while(1){
  sprintf("\x1b[1;92m"); 	
  sprintf("\nEnter choice:\n ");
  sprintf("\x1b[1;37m"); 
  
  // get a command
  memset(&bufferSize,'\0',100);
  bufferSize = 99;
  sys_req(READ, COM1,cmdBuffer,&bufferSize);

  // process command
  if (strcmp(cmdBuffer,"1") == 0){ //Get Date command
    getDate();
  }
  else if ( strcmp(cmdBuffer,"2") == 0){ //Set Date command
    setDate();
  }
  else if ( strcmp(cmdBuffer,"3") == 0){ //Get Time command
    getTime();	
  }
  else if ( strcmp(cmdBuffer,"4") == 0){ //Set Time command
    setTime();	
  }
  else if ( strcmp(cmdBuffer,"5") == 0){ // Version command
    version();
  }
  else if ( strcmp(cmdBuffer,"6") == 0){ //Help command 
    help();
  }
  else if ( strcmp(cmdBuffer,"7") == 0){ //Shutdown command
    //quit = shutdown();
    sys_req(EXIT, COM1, NULL, NULL);
  }
  else if ( strcmp(cmdBuffer,"8") == 0){ // clear command
    clear();
  }
  else if ( strcmp(cmdBuffer, "9") == 0){ //suspend

    sprintf("Enter name: \n");
    memset(&extSize,'\0',20);
    extSize = 19;
    sys_req(READ,COM1,extBuffer,&extSize);
      
    suspendPCB(extBuffer);

  }
  else if ( strcmp(cmdBuffer, "10") == 0){ //resume
    sprintf("Enter name: \n");
    memset(&extSize,'\0',20);
    extSize = 19;
    sys_req(READ,COM1,extBuffer,&extSize);
  
    resumePCB(extBuffer);

  }
  else if ( strcmp(cmdBuffer, "11") == 0){ //set priority

    char sBuffer[20];
    int sSize;
  
    sprintf("Enter name to change: \n");
    memset(&sSize,'\0',20);
    sSize = 19;
    sys_req(READ,COM1,sBuffer,&sSize);

    char suBuffer[20];
    int suSize;
    sprintf("Enter priority you wish to set: \n");
    memset(&suSize,'\0',20);
    suSize = 19;
    sys_req(READ,COM1,suBuffer,&suSize);

    char *priority = suBuffer;
      
    setPriority(sBuffer, atoi(priority));
  }
  else if ( strcmp(cmdBuffer, "12") == 0){ //show pcb
    sprintf("Enter name: \n");
    memset(&extSize,'\0',20);
    extSize = 19;
    sys_req(READ,COM1,extBuffer,&extSize);
  
    showPCB(extBuffer);
  }
  else if ( strcmp(cmdBuffer, "13") == 0){ //show all processes

    showAll();

  }
  else if ( strcmp(cmdBuffer, "14") == 0){ //show ready processes

    showReady();

  }
  else if ( strcmp(cmdBuffer, "15") == 0){ //show blocked processes

    showBlocked();

  }
  else if ( strcmp(cmdBuffer, "16") == 0){ //delete pcb
    loadr3();
  }
  else if (strcmp(cmdBuffer, "17") == 0){ //loadr3
    //alarm();
  }
  else if (strcmp(cmdBuffer, "18") == 0){ //show free
    showFree();
  }
  else if (strcmp(cmdBuffer, "19") == 0){ //show allocated
    showAllocated();
  }
  else{
  sprintf("\x1b[1;91m");  //Red error message
  sprintf("Error!: Command '");
  sprintf(cmdBuffer);
  sprintf("' is not recognized. Please try again.\n Use command '6' to see our help menu\n");
  sprintf("\x1b[1;37m"); //Default text color
}


  //if (quit == 1){
  //	break;
  //}
  
  sys_req(IDLE, DEFAULT_DEVICE, NULL, NULL);
  
  }

}
