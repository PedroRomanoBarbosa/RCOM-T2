#include "llInterface.h"
#include "linklayer.h"

int getNextNR() {
	if(ll.nr == 0) return 1;
	else if(ll.nr == 1) return 0;
}

Frame destuff(char* buf, int length) {
	Frame destuffed;

	int j = 0;
	int i;
	for(i = 0; i < length; i++) {
		if(buf[i] == ESCAPE) {
			destuffed.buf[j] = buf[i+1] ^ STUFF;
			i++;
		} else {
			destuffed.buf[j] = buf[i];
		}
		j++;
	}
	
	destuffed.length = j;
	
	return destuffed;
}

Frame stuff(char* buf, int length){
	Frame stuffed;
	int i;
	int extra = 0;
	
	for(i = 1; i < length - 1; i++) {
		if(buf[i] == FLAG || buf[i] == ESCAPE)
			extra++;
	}
	
	stuffed.length = length + extra;
	
	int j = 0;
	stuffed.buf[j] = buf[0];
	j++;
	
	for(i = 1; i < length - 1; i++) {
		if(buf[i] == FLAG) {
			stuffed.buf[j] = ESCAPE;
			j++;
			stuffed.buf[j] = FLAG ^ STUFF;
		}
		else if(buf[i] == ESCAPE) {
			stuffed.buf[j] = ESCAPE;
			j++;
			stuffed.buf[j] = ESCAPE ^ STUFF;
		}
		else {
			stuffed.buf[j] = buf[i];
		}	
		j++;
	}
	
	stuffed.buf[j] = buf[i];
	
	return stuffed;
}

int openPort(char** argv, int argc){
	struct termios newtio;

	int fd;

	if(strcmp("R",argv[2]) == 0){
		printf(" > [LL] Program type -> RECEIVER\n");
		ll.type = RECEIVER;
	} else if(strcmp("T",argv[2]) == 0){
		printf(" > [LL] Program type -> TRANSMITTER\n");
		ll.type = TRANSMITTER;
	}
	
	int timeout;
	sscanf(argv[3], "%d", &timeout);
	if(timeout <= 0){
		printf(" > [APP] Timeout must be >= 0\n");
		exit(ERROR);
	}
	ll.timeout = timeout;
	
	fd = open(argv[1], O_RDWR | O_NOCTTY);


	if (fd <0){
		perror(argv[1]);
		exit(ERROR);
	}

	if (tcgetattr(fd, &oldtio) == -1) {
		perror("tcgetattr");
		exit(-1);
	}

	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;

	newtio.c_lflag = 0;

	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN] = 0;

	tcflush(fd, TCIOFLUSH);

	if (tcsetattr(fd, TCSANOW, &newtio) == -1) {
		perror("tcsetattr");
		exit(ERROR);
	}

	printf(" > [LL] New termios structure set\n");

	return fd;
}

int closePort(){
	if (tcsetattr(ll.fd, TCSANOW, &oldtio) == -1) {
		perror("tcsetattr");
		return ERROR;
	}
	printf(" > [LL] Closing Port...\n");
	
	close(ll.fd);
	
	printf(" > [LL] Port Closed\n");
	
	return OK;
}

char* makeCMDFrame(CMDType cmd){
	char* buf = (char*)malloc(CMDLENGTH * sizeof(char));

	char addr;
	if(ll.type == TRANSMITTER){
		addr = TTOR;
	} else if(ll.type == RECEIVER) {
		addr = RTOT;
	} else {
		printf(" > [LL] Error verifying command type in makeCMDFrame\n");
		exit(ERROR);
	}
	
	buf[0] = FLAG;
	buf[1] = addr;
	buf[2] = cmd;
	buf[3] = cmd ^ addr;
	buf[4] = FLAG;
	
	return buf;
}

char* makeRESPFrame(RESPType resp){
	char* buf = malloc(CMDLENGTH);

	char addr;
	if(ll.type == TRANSMITTER){
		addr = RTOT;
	} else if(ll.type == RECEIVER){
		addr = TTOR;
	} else {
		printf(" > [LL] Error verifying command type in makeRESPFrame\n");
		exit(ERROR);
	}
	
	buf[0] = FLAG;
	buf[1] = addr;
	if(resp == RR){
		buf[2] = RR | (ll.nr << 5);
	} else if(resp == REJ){
		buf[2] == REJ | (ll.nr << 5);
	} else {
		buf[2] = resp;
	}
	buf[3] = buf[2] ^ buf[1];
	buf[4] = FLAG;

	return buf;
}

int checkRESP(RESPType cmd, char* resp){
	if(resp[0] != FLAG){
		printf(" > [LL] Error verifing position 0 in Response\n");
		return ERROR;
	}
	if(ll.type == TRANSMITTER){
		if(resp[1] != TTOR){
			printf(" > [LL] Error verifing position 1 in Response\n");
			return ERROR;
		}
	} else if(ll.type == RECEIVER){
		if(resp[1] != RTOT){
			printf(" > [LL] Error verifing position 1 in Response\n");
			return ERROR;
		}
	}
	if(cmd == UA){
		if(resp[2] != UA){
			printf(" > [LL] Error verifing position 2 in Response\n");
			return ERROR;
		}
	} else if(cmd == RR){
		if(!(resp[2] == 0x01 || resp[2] == 0x21)){
			printf(" > [LL] Error verifying position 2 RR --> %x in Response\n",resp[2]);
			return ERROR;
		}
	} else if(cmd == REJ){
		if(!(resp[2] == 0x05 || resp[2] == 0x25)){
			printf(" > [LL] Error verifying position 2 REJ --> %x in Response\n",resp[2]);
			return ERROR;
		}
	}
	if(resp[3] != (resp[2] ^ resp[1])){
		printf(" > [LL] Error verifing position 3 in Response\n");
		return ERROR;
	}
	if(resp[4] != FLAG){
		printf(" > [LL] Error verifing position 4 in Response\n");
		return ERROR;
	}
	return OK;
}

int checkCMD(CMDType command, char* cmd){
	if(cmd[0] != FLAG){
		printf(" > [LL] Error verifing position 0 in Command\n");
		return ERROR;
	}
	if(ll.type == TRANSMITTER){
		if(cmd[1] != RTOT){
			printf(" > [LL] Error verifing position 1 in Command\n");
			return ERROR;
		}
	} else if(ll.type == RECEIVER){
		if(cmd[1] != TTOR){
			printf(" > [LL] Error verifing position 1 in Command\n");
			return ERROR;
		}
	}
	if(command == SET){
		if(cmd[2] != SET){
			printf(" > [LL] Error verifing position 2 in Command\n");
			return ERROR;
		}
	} else if(command == DISC){
		if(cmd[2] != DISC){
			printf(" > [LL] Error verifing position 2 in Command\n");
			return ERROR;
		}
	}
	if(cmd[3] != (cmd[2] ^ cmd[1])){
		printf(" > [LL] Error verifing position 3 in Command\n");
		return ERROR;
	}
	if(cmd[4] != FLAG){
		printf(" > [LL] Error verifing position 4 in Command\n");
		return ERROR;
	}
	return OK;
}

int readFrame(){
	char buf[2];
	char c;
	int mstate = 0; // MACHINE STATE
	int index = 0;
	if(ll.type == TRANSMITTER){
		tcflush(ll.fd, TCIFLUSH);
		ll.frame.length = 0;
		while(!getAlarmFlag()){
			if(read(ll.fd, buf, 1)){
				c = buf[0];
				switch(mstate){
					case 0:
						if(c == FLAG){
							index = 0;
							ll.frame.buf[index] = c;
							ll.frame.length = 1;
							index = 1;
							mstate = 1;
						}
						break;
					case 1:
						if(c == TTOR || c == RTOT){
							index = 1;
							ll.frame.buf[index] = c;
							ll.frame.length = 2;
							index = 2;
							mstate = 2;
						} else if(c == FLAG){
							mstate = 1;
						} else {
							mstate = 0;
						}
						break;
					case 2:
						if(c == FLAG){
							mstate = 1;
						} else {
							ll.frame.buf[index] = c;
							ll.frame.length = 3;
							index = 3;
							mstate = 3;
						}
						break;
					case 3:
						if(c == FLAG){
							mstate = 1;
						} else {
							ll.frame.buf[index] = c;
							ll.frame.length = 4;
							index = 4;
							mstate = 4;
						}
						break;
					case 4:
						if(c == FLAG){
							ll.frame.buf[index] = c;
							ll.frame.length = 5;
							return OK;
						} else {
							mstate = 0;
						}
						break;
					}
			}
		}
	} else if(ll.type == RECEIVER){
		tcflush(ll.fd, TCIFLUSH);
		ll.frame.length = 0;
		while(!getAlarmFlag()){
			if(read(ll.fd, buf, 1)){
				c = buf[0];
				switch(mstate){
					case 0:
						if(c == FLAG){
							index = 0;
							ll.frame.buf[index] = c;
							ll.frame.length = 1;
							mstate = 1;
						}
						break;
					case 1:
						if(c == TTOR || c == RTOT){
							index = 1;
							ll.frame.buf[index] = c;
							ll.frame.length = 2;
							mstate = 2;
						} else if(c == FLAG){
							mstate = 1;
						} else {
							mstate = 0;
						}
						break;
					case 2:
						if(c == FLAG){
							mstate = 1;
						} else {
							index = 2;
							ll.frame.buf[index] = c;
							ll.frame.length = 3;
							mstate = 3;
						}
						break;
					case 3:
						if(c == FLAG){
							mstate = 1;
						} else {
							index = 3;
							ll.frame.buf[index] = c;
							ll.frame.length = 4;
							if(ll.state == CONNECTION || ll.state == TERMINATION){
								mstate = 5;
							} else if(ll.state == TRANSMISSION){
								mstate = 4;
							}
						}
						break;
					case 4: // FOR DATA TRANSMISSION
						if(c == FLAG){ // END OF RECEPTION
							index++;
							ll.frame.buf[index] = c;
							ll.frame.length++;
							return OK;
						} else {
							index++;
							ll.frame.buf[index] = c;
							ll.frame.length++;
						}
						break;
					case 5:
						if(c == FLAG){
							index = 4;
							ll.frame.buf[index] = c;
							ll.frame.length = 5;
							return OK;
						}
						break;
				}//switch
			}//if
		}//while
	}//else
	return ERROR;
}

int send(char* buf, int length){
	tcflush(ll.fd, TCOFLUSH);
	int sent = write(ll.fd, buf, length);
	if (sent == length){
		return OK;
	} else {
		return ERROR;
	}
}

int llclose(){
	alarmSetup();
	alarm(0);
	printf(" > [LL] Closing Connection...\n");
	ll.state = TERMINATION;
	int tries = 0;
	
	if(ll.type == TRANSMITTER){
		while(tries < ll.attempts){
			char* cmd = makeCMDFrame(DISC);
			printf(" > [LL] Sending DISC Command...\n");
			if(send(cmd, CMDLENGTH) != OK){
				printf(" > [LL] Error Sending DISC Command\n");
				return ERROR;
			}
			printf(" > [LL] Waiting for DISC confirmation...\n");
			setAlarmFlag(0);
			alarm(ll.timeout);
			if(readFrame() == OK){
				if(checkCMD(DISC, ll.frame.buf) == OK){
					printf(" > [LL] Received DISC confirmation\n");
					char* resp = makeRESPFrame(UA);
					printf(" > [LL] Sending UA confirmation...\n");
					if(send(resp, CMDLENGTH) != OK){
						printf(" > [LL] Error sending UA confirmation. Will now exit...\n");
						return ERROR;
					} else {
						printf(" > [LL] Sent UA confirmation. Will now close...\n");
						return closePort();
					}
				}
			}
			printf(" > [LL] Timeout\n");
			tries++;
		}
		printf(" > [LL] Max attempts exceded\n");
		return ERROR;
	} else if(ll.type == RECEIVER){
		printf(" > [LL] Waiting for DISC command...\n");
		while(1){
			setAlarmFlag(0);
			alarm(ll.timeout);
			if(readFrame() == OK){
				if(checkCMD(DISC, ll.frame.buf) == OK){
					printf(" > [LL] Received DISC command\n");
					while(tries < ll.attempts){
						char* cmd = makeCMDFrame(DISC);
						printf(" > [LL] Sending DISC confirmation...\n");
						if(send(cmd, CMDLENGTH) == OK){
							printf(" > [LL] Waiting for final UA confirmation...\n");
							setAlarmFlag(0);
							alarm(ll.timeout);
							if(readFrame() == OK){
								if(checkRESP(UA, ll.frame.buf) != OK){
									printf(" > [LL] Error checking UA confirmation\n");
									//return ERROR;
								} else {
									printf(" > [LL] Received UA confirmation. Will now exit...\n");
									return closePort();
								}
							}
						}
						tries++;
					}
				}
			}
		}
	}
}

int llopen(char** argv, int argc){
	alarmSetup();
	alarm(0);
	
	printf(" > [LL] Establishing Connection...\n");
	
	ll.fd = openPort(argv, argc);
	ll.nr = 0;
	
	if(ll.fd < 0){
		printf(" > [LL] Error opening port. Will now exit...\n");
		return ERROR;
	}
	
	printf(" > [LL] Opened port sucessfully\n");
	
	ll.state = CONNECTION;

	if(ll.type == TRANSMITTER){
		char* setCMD = makeCMDFrame(SET); // MAKE A SET COMMAND	
		int tries = 0;
		while(tries < ATTEMPTS && ll.state == CONNECTION){
			printf(" > [LL] Sending SET command...\n");
			if(send(setCMD, CMDLENGTH) != OK){
				printf(" > [LL] Error sending SET command. Will now exit\n");
				exit(ERROR);
			} else {
				printf(" > [LL] Sent SET command...\n");
				printf(" > [LL] Waiting for UA confirmation...\n");
				setAlarmFlag(0);
				alarm(ll.timeout);
				if(readFrame() == OK){ // SUCESSFULLY RECEIVED UA RESPONSE FROM RECEIVER
					if(checkRESP(UA, ll.frame.buf) == OK){
						printf(" > [LL] Connection Established\n");
						ll.state = TRANSMISSION;
						return OK;
					}
				}
				printf(" > [LL] Timeout on getting UA response\n");
			}
			tries++;
		}//while
		printf(" > [LL] Reached max number of tries. UA response not received\n");
		return ERROR;
	} else if(ll.type == RECEIVER){
		printf(" > [LL] Waiting for SET command...\n");
		while(1){
			//setAlarmFlag(0);
			//alarm(ll.timeout);
			if(readFrame() == OK){
				if(checkCMD(SET, ll.frame.buf) == OK){
					printf(" > [LL] Received SET command\n");
					char* resp = makeRESPFrame(UA);
					printf(" > [LL] Sending UA response...\n");
					if(send(resp, CMDLENGTH) != OK){
						printf(" > [LL] Error sending UA response\n");
						return ERROR;
					} else {
						printf(" > [LL] Received UA response\n");
						printf(" > [LL] Connection Established\n");
						ll.state = TRANSMISSION;
						return OK;
					}
				}
			}
		}
	}
}

RESPType checkReceivedFrame(){
	Frame temp = destuff(ll.frame.buf, ll.frame.length);
	
	printf(" > [LL] Checking received Frame...\n");

	memcpy(ll.frame.buf, temp.buf, temp.length);
	ll.frame.length = temp.length;
	
	char XOR = ll.frame.buf[1] ^ ll.frame.buf[2];
	if(ll.frame.buf[3] != XOR){
		printf(" > [LL] Detected Error in BCC1 --> A: %x | C:%x | BCC1: %x | XOR: %x\n",ll.frame.buf[1],ll.frame.buf[2],ll.frame.buf[3],XOR);
		return REJ;
	}
	
	char* tempBuf = (char*)malloc(sizeof(char)*ll.frame.length-FHT);
	memcpy(tempBuf, &ll.frame.buf[4], ll.frame.length-FHT);

	char tempBCC2 = getBCC2(tempBuf, ll.frame.length-FHT);

	if(tempBCC2 != ll.frame.buf[ll.frame.length-2]){
		printf(" > [LL] Detected Error in BCC2\n");
		return REJ;
	}
	
	return RR;
}

int llread(char** bufToApp){
	alarmSetup();
	alarm(0);
	int done = 0;
	int duplicate = 0;
	
	while(!done){
		setAlarmFlag(0);
		alarm(ll.timeout);
		if(readFrame() == OK){
			RESPType resp = checkReceivedFrame();
			if(resp == RR){
				char* response;
				if(BIT(ll.frame.buf[2],5) == ll.nr){ // all right
					ll.nr = getNextNR();
					response = makeRESPFrame(RR);
				}
				else if(BIT(ll.frame.buf[2],5) != ll.nr){ // duplicate
					printf(" > [LL] Received Duplicate Frame\n");
					response = makeRESPFrame(RR);
				}
				if(send(response, CMDLENGTH) == OK){
					printf(" > [LL] RR response -- %x -- sent\n", response[2]);
					done = 1;
					*bufToApp = (char*)malloc(sizeof(char)*(ll.frame.length - FHT));
					memcpy(*bufToApp, &ll.frame.buf[4], ll.frame.length-2);
					return (ll.frame.length - FHT);
				} else {
					printf(" > [LL] Error sending RR response...\n");
				}
			} else if(resp == REJ){
				char* response = makeRESPFrame(REJ);
				if(send(response, CMDLENGTH) == OK){
					printf(" > [LL] REJ response sent\n");
				} else {
					printf(" > [LL] Error sending REJ response...\n");
				}
			}
		}
		//printf(" > [LL] Timeout\n");
		//tries++;
	}
	//printf(" > [LL] Max number of tries reached\n");
	//exit(ERROR);
}

char getBCC2(char* buf, int length){
	char bcc2 = 0x00;
	int i;
	for(i = 0; i < length; i++){
		bcc2 = bcc2 ^ buf[i];
	}
	return bcc2;
}

Frame makeSendFrame(char* buf, int length){
	Frame temp;
	
	temp.length = length + FHT;
	
	temp.buf[0] = FLAG;
	temp.buf[1] = TTOR;
	temp.buf[2] = (ll.nr << 5);
	temp.buf[3] = temp.buf[2] ^ temp.buf[1];
	
	int i;
	for(i = 0; i < length; i++){
		temp.buf[i+4] = buf[i];
	}
	
	temp.buf[length + 4] = getBCC2(buf, length);
	temp.buf[length + 5] = FLAG;

	return stuff(temp.buf, temp.length);
}

int llwrite(char* buf, int length){
	//setAlarmFlag(0);
	alarmSetup();
	alarm(0);
	Frame toSend = makeSendFrame(buf, length);
	int tries = 0;
	
	while(tries < ATTEMPTS){
		printf(" > [LL] Sending Frame...\n");
		if(send(toSend.buf, toSend.length) != OK){
			printf(" > [LL] Unable to send Frame.");
		} else {
			printf(" > [LL] Frame sent. Waiting for confirmation...\n");
			setAlarmFlag(0);
			alarm(ll.timeout);
			if(readFrame() == OK){
				if(checkRESP(RR, ll.frame.buf) == OK){
					if(BIT(ll.frame.buf[2],5) != ll.nr){
						printf(" > [LL] Received RR -> next\n");
						ll.nr = getNextNR();
						return toSend.length;
					} else {
						printf(" > [LL] Received RR -> current\n");
					}
				} else if(checkRESP(REJ, ll.frame.buf) == OK){
					if(BIT(ll.frame.buf[2], 5) == ll.nr){
						printf(" > [LL] REJ received. Will try again.\n");
					} else {
						printf(" > [LL] REJ received for another frame...\n");
						//break;
					}
				}
			}
			printf(" > [LL] Timeout.\n");
		}
		tries++;
	}
	printf(" > [LL] Max number of tries reached\n");
	return ERROR;
}

















