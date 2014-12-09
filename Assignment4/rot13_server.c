/* CS 5785 ROT13 Server
 * 
 * Code based on examples from:
 * http://en.wikibooks.org/wiki/Serial_Programming/termios 
 */


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>

#define BAUD B115200
#define PORT "/dev/ttyO4"

int port_fd;

void sig_handler(int signo) {
	if (signo == SIGINT) {
		printf("Closing down port...\n");
        	close(port_fd);
		exit(0);
	}
}

 main(int argc,char** argv)
{
	if (signal(SIGINT, sig_handler) == SIG_ERR) {
		printf("ERROR: Setting up signal handler!\n");
		exit(1);
	}
        struct termios port_ctrl;
 
	// Open read-write, no terminal control, blocking 
        port_fd=open(PORT,O_RDWR | O_NOCTTY );      
        //port_fd=open(PORT,O_RDWR | O_NOCTTY | O_NDELAY);      
	if(port_fd  < 0) {
		printf("ERROR: Unable to open \"%s\"!\n",PORT);
		exit(1);
	}

	if(!isatty(port_fd)) {
		printf("ERROR: \"%s\" is not an avaliable port!\n",PORT);
		exit(1);
	}

	// Set for no pre-processing and recieve each character, no waiting 
	// for newline characters.
        memset(&port_ctrl,0,sizeof(port_ctrl));
        port_ctrl.c_cflag = CRTSCTS | CS8 | CLOCAL | CREAD;
        port_ctrl.c_iflag = IGNPAR;
        port_ctrl.c_oflag = 0;
        port_ctrl.c_lflag = 0;       //ICANON;
        port_ctrl.c_cc[VMIN]=42;    // Accepts up to 255 chars in a batch
        port_ctrl.c_cc[VTIME]=1;     // Read timeout of 1 sec before returning
        tcflush(port_fd, TCIFLUSH);
        tcsetattr(port_fd,TCSANOW,&port_ctrl);	


	// Set I/O baud rates
	if(cfsetispeed(&port_ctrl, BAUD) < 0 || cfsetospeed(&port_ctrl, BAUD) < 0) {
		printf("ERROR: Unable to set the baudrate!\n"); 
		exit(1);
	}
	
	// Assign config to port, flush all buffers
        if(tcsetattr(port_fd,TCSAFLUSH,&port_ctrl) < 0) {
		printf("ERROR: Unable to assign configuration to port!\n");
		exit(1);
	}

	char char_buf[255];
	memset(char_buf,0,sizeof(char_buf));

	printf("ROT13 Server Ready.    Port: \"%s\"\n",PORT);

        while (1) {
		// Using blocking I/O so the read won't return until 
		// data is recieved.

		int count = read(port_fd,&char_buf,255);
		//printf("Count = %d\n",count);
        //printf("Data = %s\n",char_buf);
		int index = 0;
		for(;index < count; index++) {
			if(char_buf[index] == 0)
				break;

			if(char_buf[index] >= 'A' && char_buf[index] <= 'Z') {
				if(char_buf[index] <= 'M') {
					char_buf[index] += 13;
				} else {
					char_buf[index] -= 13;
				}
			}
			if(char_buf[index] >= 'a' && char_buf[index] <= 'z') {
				if(char_buf[index] <= 'm') {
					char_buf[index] += 13;
				} else {
					char_buf[index] -= 13;
				}
			}
			write(port_fd,&char_buf[index],1); 
			char_buf[index] = 0;
		}
	}	

        return 0;
}
