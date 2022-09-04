/*
  ----- serial.c -----

  Description..: Contains methods and variables used for
    serial input and output.
*/

#include <stdint.h>
#include <string.h>

#include <core/io.h>
#include <core/serial.h>

#define NO_ERROR 0

// Active devices used for serial I/O
int serial_port_out = 0;
int serial_port_in = 0;

/*
  Procedure..: init_serial
  Description..: Initializes devices for user interaction, logging, ...
*/
int init_serial(int device)
{
  outb(device + 1, 0x00); //disable interrupts
  outb(device + 3, 0x80); //set line control register
  outb(device + 0, 115200/9600); //set bsd least sig bit
  outb(device + 1, 0x00); //brd most significant bit
  outb(device + 3, 0x03); //lock divisor; 8bits, no parity, one stop
  outb(device + 2, 0xC7); //enable fifo, clear, 14byte threshold
  outb(device + 4, 0x0B); //enable interrupts, rts/dsr set
  (void)inb(device);      //read bit to reset port
  return NO_ERROR;
}

/*
  Procedure..: serial_println
  Description..: Writes a message to the active serial output device.
    Appends a newline character.
*/
int serial_println(const char *msg)
{
  int i;
  for(i=0; *(i+msg)!='\0'; i++){
    outb(serial_port_out,*(i+msg));
  }
  outb(serial_port_out,'\r');
  outb(serial_port_out,'\n');
  return NO_ERROR;
}

/*
  Procedure..: serial_print
  Description..: Writes a message to the active serial output device.
*/
int serial_print(const char *msg)
{
  int i;
  for(i=0; *(i+msg)!='\0'; i++){
    outb(serial_port_out,*(i+msg));
  }
  if (*msg == '\r') outb(serial_port_out,'\n');
  return NO_ERROR;
}

/*int (*student_read)(char *buffer, int *count);
  Procedure..: set_serial_out
  Description..: Sets serial_port_out to the given device address.
    All serial output, such as that from serial_println, will be
    directed to this device.
*/
int set_serial_out(int device)
{
  serial_port_out = device;
  return NO_ERROR;
}

/*
  Procedure..: set_serial_in
  Description..: Sets serial_port_in to the given device address.
    All serial input, such as console input via a virtual machine,
    QEMU/Bochs/etc, will be directed to this device.
*/
int set_serial_in(int device)
{
  serial_port_in = device;
  return NO_ERROR;
}
//Polling

	// left arrow: [D
	// right arrow: [C
	// enter: \n
	// backspace: 127
	// delete: [3

	/* special keys:

		backspace: moves cursor back one while deleting character directly behind cursor
		arrow keys (left / right: changes lines of cursor or placement of cursor
		enter key: go to next line and enter input in for processing

	*/
// cursor is location to be typed in next

//Questions:
	// how to remove from buffer
	// how to adjust buffer/ move around buffer
  // doesn't input normal character (i.e. a-zA-Z0-9)
  //

int *polling(char *buffer, int *count){
// insert your code to gather keyboard input via the technique of polling.
// You must validat each key and handle special keys such as delete, back space, and
// arrow keys

int index = 0;
int size = 0;
int i = 0;
while(1){


	if (inb(COM1 + 5) & 1){ 		//shows that character is available
	char input = inb(COM1);  	// sets available character
		if(input == '\x1b'){   // checks if special key
			input = inb(COM1);
			if(input == '['){ // arrow key
				input = inb(COM1);
         if(input == '3'){
           input = inb(COM1);
           if(input == '~'){
           int i = index;
           if (i <= size){
             buffer[index] = buffer[index + 1];
             i++;
             size--;
           }
           serial_print("\x1b[2K");
           serial_print("\x1b[0G");
           serial_print(buffer);
          }
         }
					switch(input){
						case 'D' : // checks case for left arrow key
							if (index !=0){
                serial_print("\x1b[1D"); // move left
								index--;
							} 	// adjust buffer
							break;
						case 'C' : // checks case for right arrow key
            if(size > index){
							serial_print("\x1b[1C"); // moves right
								index++;
							  // adjust buffer
            }
            	break;
					}
			}
		}
	  else if(input == '\r' || input == '\n'){ //enter key
			serial_print("\n"); // next line
      return count;
		}

		else if(input == 127){ //backspace
      if (index !=0){
      i = index;
      while(i <= size){
        buffer[index - 1] = buffer[index];
        i++;
      }
      serial_print("\x1b[2K");
      serial_print("\x1b[0G");
      serial_print(buffer);
				index--;
				size--;
			}
		}
    else if(input >= 32 && input <= 125){ // checks for normal character a-z A-Z 0-9
      if(index <= size){
        buffer[index] = buffer[index + 1];
      }
      size++;
      buffer[index] = input;
      serial_print(&buffer[index]);
      index++;
    }
	  //if letter or number print out character

	}
		//serial_print(&input);

}

// remove the following line after implementing your module, this is present
// just to allow the program to compile before R1 is complete
strlen(buffer);

return count;
}
