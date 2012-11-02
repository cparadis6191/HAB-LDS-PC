#include <iostream>
#include <windows.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>

using namespace std;


int main(){
	wstring serialPort;
	HANDLE hSerial;
	
	wcout<<L"Enter which Serial Port to connect to: ";
	wcin>>serialPort;
	wstring prefix=L"\\\\.\\";
	wstring ID=prefix+serialPort;
	
	hSerial=CreateFileW(ID.c_str(),					//Name of device to being openned
					   GENERIC_READ|GENERIC_WRITE,	//Access writes (Read and Write)
					   0,							//Prevent other process from accessing the file
					   0,							//Boolean value to determine whether handle can be inherited
					   OPEN_EXISTING,				//Opens device only if it exists
					   FILE_ATTRIBUTE_NORMAL,		//Sets file attributes (Set to standard)
					   0);							//Ignored value since opening an existing file (Only for file creation)

	if(hSerial==INVALID_HANDLE_VALUE){
		if(GetLastError()==ERROR_FILE_NOT_FOUND){
			cout<<"Specified Serial Port does not exist\n";
			CloseHandle(hSerial);
			exit(EXIT_FAILURE);
		}
		else{
			cout<<"Unspecifed Error:"<<GetLastError()<<endl;
			CloseHandle(hSerial);
			exit(EXIT_FAILURE);			
		}
	}
													//Begin setting serial communication parameters	
	DCB dcbSerial={0};								//Set all DCB fields to 0
	
	dcbSerial.DCBlength=sizeof(dcbSerial);			//Sets the length of the structure in bytes
	
	if(!GetCommState(hSerial,&dcbSerial)){
		cout<<"Error retrieving communication state\n";
		CloseHandle(hSerial);
		exit(EXIT_FAILURE);
	}
	
	dcbSerial.BaudRate=CBR_9600;					//Set important serial communication variables
	dcbSerial.ByteSize=8;
	dcbSerial.StopBits=ONESTOPBIT;
	dcbSerial.Parity=NOPARITY;
	
	if(!SetCommState(hSerial,&dcbSerial)){
		cout<<"Error setting communication state.\n";
		CloseHandle(hSerial);
		exit(EXIT_FAILURE);
	}
	
	COMMTIMEOUTS timeout={0};
	
	timeout.ReadIntervalTimeout=50;
	timeout.ReadTotalTimeoutConstant=50;
	timeout.ReadTotalTimeoutMultiplier=10;
	timeout.WriteTotalTimeoutConstant=50;
	timeout.WriteTotalTimeoutMultiplier=10;
	
	if(!SetCommTimeouts(hSerial,&timeout)){
		cout<<"Error setting timeout parameters.\n";
		CloseHandle(hSerial);
		exit(EXIT_FAILURE);
	}
	
	
	
	/*char writeBuffer[1]={0};
	writeBuffer[0]=0x56;
	writeBuffer[1]='5';
	DWORD BytesWrite=0;
	if(!WriteFile(hSerial,writeBuffer,1,&BytesWrite,NULL)){
		cout<<"Error occured while writing.\n";
		CloseHandle(hSerial);
	}
	
	char readBuffer[1]={0};
	DWORD BytesRead=0;
	
	if(!ReadFile(hSerial,readBuffer,1,&BytesRead,NULL)){
		cout<<"Error occured while reading.\n";
		CloseHandle(hSerial);
		exit(EXIT_FAILURE);
	}
	cout<<readBuffer[0];*/
	
	CloseHandle(hSerial);
	
	return 0;
}