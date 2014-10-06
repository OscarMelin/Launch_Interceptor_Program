#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
 
int main(int argc,char** argv)
{
        struct termios port_ctrl;
        int port_fd;
	char* port = "/dev/ttyO4";
 
	// Open read-write, no terminal control, nonblocking 
        port_fd=open(port,O_RDWR | O_NOCTTY | O_NDELAY);      
	if(port_fd  < 0) {
		printf("ERROR: Unable to open \"%s\"!\n",port);
	}

	if(!isatty(port_fd)) {
		printf("ERROR: \"%s\" is not an avaliable port!\n",port);
	}

        unsigned char char_buf = ' ';
 
        memset(&port_ctrl,0,sizeof(port_ctrl));
        port_ctrl.c_cflag = CRTSCTS | CS8 | CLOCAL | CREAD;
        port_ctrl.c_iflag = IGNPAR;
        port_ctrl.c_oflag = 0;
        port_ctrl.c_lflag = 0;       //ICANON;
        port_ctrl.c_cc[VMIN]=1;
        port_ctrl.c_cc[VTIME]=0;
        tcflush(port_fd, TCIFLUSH);
        tcsetattr(port_fd,TCSANOW,&port_ctrl);	


	// Set I/O baud rates
	if(cfsetispeed(&port_ctrl, B9600) < 0 || cfsetospeed(&port_ctrl, B9600) < 0) {
		printf("ERROR: Unable to set the baudrate!\n"); 
	}
	
	// Assign config to port, flush all buffers
        if(tcsetattr(port_fd,TCSAFLUSH,&port_ctrl) < 0) {
		printf("ERROR: Unable to assign configuration to port!\n");
	}

	printf("Port is open, echoing until newline. \n");
	// Loops until a newline is sent
        while (char_buf!='\n') {
                if (read(port_fd,&char_buf,1)>0) { // If data on serial port
			printf("Char: %c\n",char_buf);
			write(port_fd,&char_buf,1); // Echo it back out manually
		}
	}	

	printf("Port closed, newline character recieved. \n");
        close(port_fd);
        return 0;
}
