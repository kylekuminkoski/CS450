
//#ifndef _GETTIME_H
//#define _GETTIME_H

#include <system.h>

//int decimal(char bcd);

//int BCD(int dec);

int getTime();

int setTime();

void version();

void help();

int shutdown();

int getDate();

int setDate();

int clear();

char* itoa(int value, char* buffer, int base);
