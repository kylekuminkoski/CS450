#include "time.h"
#include <stdint.h>
#include <string.h>
#include <system.h>

#include <core/io.h>
#include <core/serial.h>
#include "mpx_supt.h"

//0x70 - location
//0x71 - value

//absolute value function
int abs(int x){
  if(x<0){
    return -x; //if value negative, return positive
  }
  else{
    return x; //value already positive
  }
}

//swap function
void swap(char *x, char *y){
  char temp = *x;
  *x = *y;
  *y = temp;
}

//reverse function
char* reverse(char str[], int i, int j){
  while(i<j){ //while first half is less than second half of array
    swap(&str[i++], &str[j--]);
  }
  
  return str;
}

//integer to char, converts to base 2,8,10,or 16 + adds to buffer
char* itoa(int value, char* buffer, int base){
  int n = abs(value);
  
  int i = 0;
  
  while(n){
    int r = n % base;
    
    if(r >= 10){
      buffer[i++] = 65 + (r-10);
    }
    else{
      buffer[i++] = 48 + r;
    }
    
    n = n / base;
  }
  
  if(i == 0){
    buffer[i++] = '0';
  }
  
  if(value < 0 && base == 10){
    buffer[i++] = '-';
  }
  
  buffer[i] = '\0';
  
  return reverse(buffer,0,i-1);
  
}

//function that returns system time
int getTime(){
  char sBuffer[15], mBuffer[15], hBuffer[20];
  unsigned char s, m, h;
  int seconds = 0, minutes = 0, hours = 0;
  char *sec, *min, *hr;
  
  //hours////// set time fixes issue, 4 hours ahead
  outb(0x70,0x04); //get from location 0x70
  h = inb(0x71); //get value from 0x71
  
  hours += (h/16) * 10; //fix bcd
  hours += h&0x0F; 
  hr = itoa(hours, hBuffer, 10); //itoa to base 10
  
  //minutes
  outb(0x70,0x02);
  m = inb(0x71);
  minutes += (m/16) * 10;  
  minutes += m&0x0F; 
  min = itoa(minutes, mBuffer, 10);
  
  //seconds
  outb(0x70,0x00);
  s = inb(0x71);
  seconds += (s/16) * 10;
  seconds += s&0x0F;  
  sec = itoa(seconds, sBuffer, 10);
  
  
  sprintf(hr);
  sprintf(":");
  sprintf(min);
  sprintf(":");
  sprintf(sec);
  sprintf("\n");
  
  return 0;
}

//function that sets the time of the system
int setTime(){
  char cmdBuffer[100];
  int bufferSize;

  char hVar[10], mVar[10], sVar[10];
  int hours = 0, minutes = 0, seconds = 0;
  int h, m, s;
  
  sprintf("Please enter using military time\n");
  
  //hours
  sprintf("Enter the hour: ");
  
  memset(&bufferSize,'\0',100); //process user input
  bufferSize = 99;
  sys_req(READ,COM1,cmdBuffer,&bufferSize);
  
  h = atoi(cmdBuffer); //atoi to check input is correct
  
  if(h > 24 || h < 0){ //check if input is correct
    sprintf("Please type it in the correct format!");
    return 0;
  }
  
  h = atoi(itoa(h, hVar, 2)); //itoa to convert to base 2, atoi back to integer
  hours = (h/10) * 16; //fix bcd
  hours = h&0x0F;
  
  //minutes
  sprintf("Enter the minutes: ");
  
  memset(&bufferSize,'\0',100);
  bufferSize = 99;
  sys_req(READ, COM1,cmdBuffer,&bufferSize);
  
  m = atoi(cmdBuffer);
  
  if(m > 60 || m < 0){
    sprintf("Please type it in the correct format!\n");
    return 0;
  }

  m = atoi(itoa(m, mVar, 2));
  minutes = (m/10) * 16;
  minutes = m&0x0F;
  
  //seconds
  sprintf("Enter the seconds: ");
  
  memset(&bufferSize,'\0',100);
  bufferSize = 99;
  sys_req(READ, COM1,cmdBuffer,&bufferSize);
  
  s = atoi(cmdBuffer);
  
  if(s > 60 || s < 0){
    sprintf("Please type it in the correct format!\n");
    return 0;
  }
 
  s = atoi(itoa(s, sVar, 2));
  seconds = (s/10) * 16;
  seconds = s&0x0F;
  
  //input into system
  cli(); //disable interrupts
  
  outb(0x70, 0x04); //wiring into appropriate location
  outb(0x71, hours); //change value by writing to port 0x71
  
  outb(0x70, 0x02);
  outb(0x71, minutes);
  
  outb(0x70, 0x00);
  outb(0x71, seconds);

  sti(); //enable interrupts
  
  return 0;
}

int setDate(){
  char cmdBuffer[100];
  int bufferSize;

  char day[10], month[10], year[10];
  int days = 0, months = 0, years = 0;
  int d, m, y;
 
  //days
  sprintf("Enter the day: ");
 
  memset(&bufferSize,'\0',100);
  bufferSize = 99;
  sys_req(READ, COM1,cmdBuffer,&bufferSize);
  
  d = atoi(cmdBuffer);
 
  if(d > 31 || d < 0){
    sprintf("Please type a correct day\n");
    return 0;
  }

  d = atoi(itoa(d, day, 2));
  days = (d/10) * 16;
  days = d&0x0F;

  //months
  sprintf("Enter the month: ");
 
  memset(&bufferSize,'\0',100);
  bufferSize = 99;
  sys_req(READ, COM1,cmdBuffer,&bufferSize);
  
  m = atoi(cmdBuffer);
 
  if(m > 12 || m < 0){
    sprintf("Please type a correct month\n");
    return 0;
  }
  
  m = atoi(itoa(m, month, 2));
  months = (m/10) * 16;
  months = m&0x0F;

  //years
  sprintf("Enter the year: ");
 
  memset(&bufferSize,'\0',100);
  bufferSize = 99;
  sys_req(READ, COM1,cmdBuffer,&bufferSize);
  
  y = atoi(cmdBuffer);
 
  if(y > 2025 || y < 0){
    sprintf("Please type a year that exists in modern history\n");
    return 0;
  }
  
  y = atoi(itoa(y, year, 2));
  years = (y/10) * 16;
  years = y&0x0F;
  
 
  //input into system
  cli(); //disable interrupts
 
  outb(0x70, 0x07);
  outb(0x71, days); 
 
  outb(0x70, 0x08);
  outb(0x71, months);
 
  outb(0x70, 0x09);
  outb(0x71, years);

  sti(); //enable interrupts
 
  return 0;
}

int getDate(){
  char dBuff[10], mBuff[10], yBuff[10];
  char *days, *months, *years;
  int d, m, y;
  int tempDay = 0, tempMonth = 0, tempYear = 0;
 
  //days
  outb(0x70, 0x07);
  d = inb(0x71);
  tempDay += (d/16) * 10; 
  tempDay += d&0x0F; 
  days = itoa(tempDay, dBuff, 10);
 
  //months
  outb(0x70, 0x08);
  m = inb(0x71);
  tempMonth += (m/16) * 10; 
  tempMonth += m&0x0F; 
  months = itoa(tempMonth, mBuff, 10);
   
  //years
  outb(0x70, 0x09);
  y = inb(0x71);
  tempYear += (y/16) * 10; 
  tempYear += y&0x0F; 
  years = itoa(tempYear, yBuff, 10);

  //convert back to char for sprintf
  sprintf(months);
  sprintf("/");
  sprintf(days);
  sprintf("/");
  sprintf(years);
  sprintf("\n");
 
  return 0;
 }


