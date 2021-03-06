#include "PC.h"


using namespace std;


int main(){

	char swtch='0';									//Switch for switch case menu
	int poll=0;										//Used for setting polling interval

	ofstream datafile;								//Datastream used for retrieving datafiles

	char writeBuffer[1]={0},readBuffer[1]={0};		//Needed for writing and reading serially
	DWORD BytesWritten=0,BytesRead=0;

	wstring serialPort;								//Needed for opening a serial port
	HANDLE hSerial;
		
//Begin connection to serial port
	cout<<"Enter which Serial Port to connect to: ";
	wcin>>serialPort;
	wstring prefix=L"\\\\.\\";
	wstring ID=prefix+serialPort;
	
/*	COMMTIMEOUTS timeout={0};						//Set so serial port timesout if no data is being sent
													//Stops program from hanging while waiting for nonexistant data
	timeout.ReadIntervalTimeout=50;
	timeout.ReadTotalTimeoutConstant=50;
	timeout.ReadTotalTimeoutMultiplier=10;
	timeout.WriteTotalTimeoutConstant=50;
	timeout.WriteTotalTimeoutMultiplier=10;
	
	if(!SetCommTimeouts(hSerial,&timeout)){			//Set timeout settings, send error if can't be set
		cout<<"Error setting timeout parameters.\n";
		CloseHandle(hSerial);
		exit(EXIT_FAILURE);
	}*/

//Begin PC/AVR communication	

//Begin setting polling or recieving data
	while(1){

													//Give switch-case options
		cout<<"\nPress 'R' to begin recieving data from the device.\n";
		cout<<"Press 'S' to begin sending a polling interval to the device.\n";
		cout<<"Press 'C' to clear memory.\n";
		cout<<"Press 'E' to close out of the program.\n";
		cin>>swtch;
		
		
		
		{
		hSerial=CreateFileW(ID.c_str(),					//Name of device to being openned
				   GENERIC_READ|GENERIC_WRITE,	//Access writes (Read and Write)
				   0,							//Prevent other process from accessing the file
				   0,							//Boolean value to determine whether handle can be inherited
				   OPEN_EXISTING,				//Opens device only if it exists
				   FILE_ATTRIBUTE_NORMAL,		//Sets file attributes (Set to standard)
				   0);							//Ignored value since opening an existing file (Only for file creation)

		if(hSerial==INVALID_HANDLE_VALUE){				//Error if an invalid serial port is input
			if(GetLastError()==ERROR_FILE_NOT_FOUND){
				cout<<"Specified Serial Port does not exist\n";
				CloseHandle(hSerial);
				exit(EXIT_FAILURE);
			}	
			else{										//Error incase of unexpected errors
				cout<<"Unspecifed Error:"<<GetLastError()<<endl;
				CloseHandle(hSerial);
				exit(EXIT_FAILURE);			
			}
		}
														//Begin setting serial communication parameters	
		DCB dcbSerial={0};								//Set all DCB fields to 0
		
		dcbSerial.DCBlength=sizeof(dcbSerial);			//Sets the length of the structure in bytes
		
		if(!GetCommState(hSerial,&dcbSerial)){			//Get current comm settings, send error if they can't be retrieved
			cout<<"Error retrieving communication state\n";
			CloseHandle(hSerial);
			exit(EXIT_FAILURE);
		}
		
		dcbSerial.BaudRate=CBR_9600;					//Set important serial communication variables
		dcbSerial.ByteSize=8;
		dcbSerial.StopBits=ONESTOPBIT;
		dcbSerial.Parity=NOPARITY;
		
		if(!SetCommState(hSerial,&dcbSerial)){			//Set new comm settings, send error if they can't be set
			cout<<"Error setting communication state.\n";
			CloseHandle(hSerial);
			exit(EXIT_FAILURE);
		}
		}
		
		
		
		switch(swtch){
		
		case 'S':									//Give options for different polling intervals
			cout<<"\nPress '0' to set the polling interval to one second.\n";
			cout<<"Press '1' to set polling interval to five seconds.\n";
			cout<<"Press '2' to set polling interval to ten seconds.\n";
			cout<<"Press '3' to set polling interval to fifteen seconds.\n";
				   
			cin>>poll;
			
			
			cout<<"Waiting to make connection\n";			//Sends the byte 0XAB to inform AVR connection is being attempted
			
			writeBuffer[0]=0XAB;
			sendByte(writeBuffer,hSerial,&BytesWritten);

			cout<<"Init byte sent.\n";
			while(readBuffer[0]!=(char)0xBB){						//Wait until byte 0XBB is recieved from AVR to know it is ready to commuicate
				getByte(readBuffer,hSerial,&BytesRead);
				/*if(!ReadFile(hSerial,readBuffer,1,&BytesRead,NULL)){
					cout<<"Error occured while reading.\n";
					CloseHandle(hSerial);
					exit(EXIT_FAILURE);
				}*/
			}
			cout<<"Ready byte recieved";
						
			if((poll>=0)&&(poll<4)){					//Checks if polling interval is valid
				writeBuffer[0]=0xEB;				//If valid send 0XEB to let AVR know a polling interval is coming
				sendByte(writeBuffer,hSerial,&BytesWritten);				
				if(poll==0) writeBuffer[0]=1;
				else writeBuffer[0]=poll*5;				//Followed by the actual polling interval
				sendByte(writeBuffer,hSerial,&BytesWritten);
				cout<<"\nPolling interval has been sent.\n";
			}
			else{									//If input interval isn't valid prompt user
				cout<<"Invalid input.";
			}
			break;
		
		case 'R':									//Begin reading in data
			
			cout<<"Waiting to make connection\n";			//Sends the byte 0XAB to inform AVR connection is being attempted
			writeBuffer[0]=0XAB;
			sendByte(writeBuffer,hSerial,&BytesWritten);
			cout<<"Init byte sent.\n";
			while(readBuffer[0]!=(char)0xBB){						//Wait until byte 0XBB is recieved from AVR to know it is ready to commuicate
				getByte(readBuffer,hSerial,&BytesRead);
				/*if(!ReadFile(hSerial,readBuffer,1,&BytesRead,NULL)){
					cout<<"Error occured while reading.\n";
					CloseHandle(hSerial);
					exit(EXIT_FAILURE);
				}*/
			}
			cout<<"Ready byte recieved";
			
			cout<<"\nReading in data, do not disconnect device.\n";
			datafile.open("data.txt");				//Opens a file to write incoming data to
			writeBuffer[0]=0xCB;					//Send 0XCB to AVR indicating that PC is wating for data
			sendByte(writeBuffer,hSerial,&BytesWritten);
			while(1){				//Read in data until 0XDB is sent indicating all data has been sent
				getByte(readBuffer,hSerial,&BytesRead);
				/*if(!ReadFile(hSerial,readBuffer,1,&BytesRead,NULL)){
					cout<<"Error occured while reading.\n";
					CloseHandle(hSerial);
					exit(EXIT_FAILURE);
				}*/
				if(readBuffer[0]==(char)0xDB){
					break;
				}else{
					datafile<<readBuffer[0];			//Write data to file byte by byte
				}
			}
			datafile.close();						//Once all data has been read close file and prompt user.
			cout<<"Data successfully writtien to \"data.txt\"";
			break;
		
		case 'E':									//On quit close communication handle and return
						
	
			
			return 0;
		
		case 'C':									//On quit close communication handle and return
			
			cout<<"Waiting to make connection\n";			//Sends the byte 0XAB to inform AVR connection is being attempted
			writeBuffer[0]=0XAB;
			sendByte(writeBuffer,hSerial,&BytesWritten);
			cout<<"Init byte sent.\n";
			while(readBuffer[0]!=(char)0xBB){						//Wait until byte 0XBB is recieved from AVR to know it is ready to commuicate
				getByte(readBuffer,hSerial,&BytesRead);
				if(!ReadFile(hSerial,readBuffer,1,&BytesRead,NULL)){
					cout<<"Error occured while reading.\n";
					CloseHandle(hSerial);
					exit(EXIT_FAILURE);
				}
			}
			cout<<"Ready byte recieved";
			
			writeBuffer[0]=0xAC;					//Send 0XAC to AVR indicating that communication is being closed
			sendByte(writeBuffer,hSerial,&BytesWritten);
			
			break;
		
		default:									//On invalid input prompt user.
			cout<<"\nInvalid input\n";
			break;
		}
		CloseHandle(hSerial);
	}
}

void sendByte(char *writeByte,HANDLE hSerial,DWORD *BytesWritten){
	if(!WriteFile(hSerial,writeByte,1,BytesWritten,NULL)){
		cout<<"Error occured while writing.\n";
		CloseHandle(hSerial);
	}
	return;
}

void getByte(char *readBuffer,HANDLE hSerial,DWORD *BytesRead){
	if(!ReadFile(hSerial,readBuffer,1,BytesRead,NULL)){
		cout<<"Error occured while reading.\n";
		CloseHandle(hSerial);
		exit(EXIT_FAILURE);
	}
}