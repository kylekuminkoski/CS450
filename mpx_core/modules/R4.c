#include <stdint.h>
#include <string.h>
#include <system.h>

#include <core/io.h>
#include <core/serial.h>
#include "mpx_supt.h"
#include "pcb.h"
#include "time.h"

void alarm(){

  char cmdBuffer[100];
  int bufferSize;
  char hours, minutes, seconds;

  int h, m, s;
  
  sprintf("Please enter using military time\n");

//hours
  sprintf("Enter the hour: ");
  
  memset(&bufferSize,'\0',100); //process user input
  bufferSize = 99;
  sys_req(READ,COM1,cmdBuffer,&bufferSize);
  
  time *alarmTime;
  alarmTime = (time*)sys_alloc_mem(sizeof(time));
  hours = cmdBuffer;
  alarmTime->hr = hours;
  

  h = atoi(cmdBuffer); //atoi to check input is correct
  
  if(h > 24 || h < 0){ //check if input is correct
    sprintf("Please type it in the correct format!");
    return;
  }
  
 
  
  //minutes
  sprintf("Enter the minutes: ");
  
  memset(&bufferSize,'\0',100);
  bufferSize = 99;
  sys_req(READ, COM1,cmdBuffer,&bufferSize);
 	minutes = cmdBuffer;
  alarmTime->min = minutes;
  
  m = atoi(cmdBuffer);
  
  if(m > 60 || m < 0){
    sprintf("Please type it in the correct format!\n");
    return;
  }

  
  
 //seconds
  sprintf("Enter the seconds: ");
  
 memset(&bufferSize,'\0',100);
  bufferSize = 99;
  sys_req(READ, COM1,cmdBuffer,&bufferSize);
  seconds = cmdBuffer;
  alarmTime->sec = seconds;
  
  s = atoi(cmdBuffer);
  
  if(s > 60 || s < 0){
    sprintf("Please type it in the correct format!\n");
    return;
  }
  
   sprintf(alarmTime->hr);
  sprintf(":");
  sprintf(alarmTime->min);
  sprintf(":");
  sprintf(alarmTime->sec);
  sprintf("\n");
 
  

	return;
}
