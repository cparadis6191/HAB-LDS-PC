#ifndef PC_H
#define PC_H

#include <iostream>
#include <windows.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

void sendByte(char *writeByte,HANDLE hSerial,DWORD *BytesWritten);
void getByte(char *readBuffer,HANDLE hSerial,DWORD *BytesRead);

#endif