#include <iostream>
#include <windows.h>
#include <string>

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
		}
		else{
			cout<<"Unspecifed Error:"<<GetLastError()<<endl;	
		}
	}
	
	DCB dcbSerial={0};								//Begin setting serial communication parameters
	
	dcbSerial.DCBlength=sizeof(dcbSerial);			//Sets the length of the structure in bytes
	
	if(GetCommState(hSerial,&dcbSerial)==0){
		cout<<"Error retrieving communication state";
	}
	return 0;
}