#include "dcb.h"
#include "serial.h"
#include <core/serial.h>
#include <core/io.h>
#include <core/tables.h>
#include <string.h>

u32int original_idt_entry;

u32int dev = COM1;
int level = 4;

u32int dev2 = COM2;
int level2 = 3;

struct dcb serial_dcb = {
  .ring_s = sizeof(*(serial_dcb.ring)/sizeof(unsigned char))
};

void set_int(int bit, int on){
  if(on == 1){
    outb(dev + 1, inb(dev + 1) | (1<<bit));
  }
  else{
    outb(dev + 1, inb(dev + 1) & -(1<<bit)); 
  }
}

void top_handler(){
  klogv("int");
  outb(dev, 'b');
 
  if(serial_dcb.open == 1){
    cli();
 
    int type = inb(dev + 2);
    int bit1 = type>>1 & 1;
    int bit2 = type>>2 & 1;
    
    //clear specific interrupts
    if(!bit1 && !bit2){
      //modem
      inb(dev + 6);
    }
    else if(bit1 && !bit2){
      //output
      //none, already cleared by reading the interrupt id register (dev + 2)
    }
    else if(!bit1 && bit2){
      //input
      //inb(dev + 1);
    }
    else if(bit1 && bit2){
      //line status register
      inb(dev + 5);
    }
    
    sti();
  }
  
  outb(0x20, 0x20);
}

void bottom_handler(){
//input interrupt

  //read character from input register
  
  //if current status is not readying, store character in ring buffer
  //if buffer is full, discard character
  //in either case return to top_handler, do not signal completion
  
  //ow/ current status reading. store character in the requestors input buffer
  
  //if the count is not completed and character is not cr, return. do not signal completion
  
  //
  
//output interrupt
  //if status not writing, ignore and return
  
  //ow/ if count not been exhausted
  //get the next char from the requestors output buffer and store it in output register
  //return without signaling completion
  
  //ow/ all characters have been transfered. reset status to idle.
  //set event flag and return count value.
  //disable write interrupts by clearing bit 1 in the interrupt enable register  
}


int com_open(int baud_rate){
  //ERROR CHECKING
  //baud rate valid
  //port is not open
  
  //if(serial_dcb.open == 1){
  //  return -103;
  //}
  
  cli();
  
  //initialize dcb
  serial_dcb.open = 1;
  serial_dcb.events = 1;
  serial_dcb.status = NOTHING;
  
  //save address of current interrupt vector
  original_idt_entry = idt_get_gate(0x24);
  
  //install new handler in the interrupt vector
  idt_set_gate(0x24, (u32int) top_handler, 0x08, 0x8e);
  
  //compute baud rate divisor
  long brd = 115200/(long) baud_rate;
  
  //disable ints
  outb(dev + 1, 0b00000000);
  
  //set line control register
  outb(dev + 3, 0b10000000);
  
  //lsb baud_rate
  outb(dev + 0, brd);
  
  //msb baud_rate
  outb(dev + 1, brd >> 8);
  
  //lock divisor
  outb(dev + 3, 0b00000011);
  
  //enable fifo, clear, 14 byte threshold)
  outb(dev + 2, 0b11000111);
  
  //enable PIC level
  outb(0x21, inb(0x21) & -(1<<level));
  
  outb(dev + 1, 0b00000001);
  (void) inb(dev);
  
  sti();
  
  return 0;
  
}

/*
int com_close(void){
  //ensure port is open
  if(!serial_dcb.open){
    return -201;
  }
  
  //clear the open indicator in the dcb
  serial_dcb.open = false;
  
  //disable the appropriate level in the PIC mask register
  outb(0x21, inb(0x21) & -(0<<level)); //?
  
  //disable all interrupts in the acc by loading zero values to the 
  //modem status register and the interrupts
  
  //restore the original saved interrupt vector
  
  
  return 0; //success
}

int com_read(char *buf_p, int *count_p){
  //validate the supplied parameters
  
  //ensure port is open and status is idle
  if(!serial_dcb.open){
    return -301;
  }
  if else(!serial_dcb.status == NOTHING){
    return -304;
  }
  
  //initialize the input buffer variables (not ring buffer) and set the status to reading
  serial_port.in_x = buf_p; 
  serial_port.in_s = count_p; 
  serial_dcb.status = READING;
  
  //clear the callers event flag
  serial_port.event = 0;
  
  //copy characters from ring buffer to requestors buffer, until the ring buffer is:
  //emptied, requested count reached, or a CR(ENTER) code has been found.
  //copied characters should be removed from the ring buffer.
  //either input interrupts or all interrupts should be disabled during the copying
  
  
  //if more characters are needed, return. if block is complete, continue to next step
  
  
  //reset the dcb status to idle, set event flag and return the actual count to the
  //requestors variable
  serial_dcb.status = NOTHING;
  
  
}

int com_write(char *buf_p, int *count_p){
  //ensure the inputs parameters are valid
  
  //ensure that the port is currently open and idle 
   if(!serial_dcb.open){
    return -401;
  }
  if else(!(serial_dcb.status == NOTHING)){
    return -404;
  }
  
  //install the buffer pointer and counters in the dcb, and set the current status to writing
  
  serial_port.status = WRITING;
  
  //clear the callers event flag
  serial_port.event = 0;
  
  //get the first character from the requestors buffer and store it in the output register
  
  //enable the write interrupts by setting bit 1 of the interrupt enable register, this must be done by setting the register to the logical or of its previous contents and 0x02
 
}

*/


