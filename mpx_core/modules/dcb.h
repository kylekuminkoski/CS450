#ifndef _DCB_H_
#define _DCB_H_

#include <stdint.h>
#include <string.h>
#include <system.h>

#include <core/io.h>
#include <core/serial.h>
#include <core/tables.h>
#include <core/interrupts.h>
#include <mem/heap.h>
#include <mem/paging.h>


enum dcb_status {NOTHING, READING, WRITING};

struct dcb{
  //flag indicating whether open or closed
  // 0 - closed
  // 1 - open;
  int open;
  
  //flag indicating completion of operation
  //0 at beginning of operation, 1 at end
  int events;
  
  enum dcb_status status;
  
  //input buffer
  //stores information FROM device requested by application
  //in_x: current index, in_s: size of 'in' buffer
  unsigned char* in;
  int in_x, in_s;
  
  //output buffer
  //stores information FOR device requested by application
  //out_x: current index, in_s: size of 'in' buffer
  unsigned char* out;
  int out_x, out_s;
  
  //ring buffer 
  //written to whenever the device triggers an interrupt. ow/ we might
  //miss data from the device before we query for input again
  unsigned char ring[16];
  int ring_inx, ring_outx, ring_s;
};

#endif
