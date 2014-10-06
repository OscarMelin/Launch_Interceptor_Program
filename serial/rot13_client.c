 
       



#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>

#define BAUD B115200
#define PORT "/dev/ttyO1"

int port_fd;
struct termios old_stdio;

void sig_handler(int signo) {
	if (signo == SIGINT) {
		printf("Closing down port...\n");
        	close(port_fd);
        	tcsetattr(STDOUT_FILENO,TCSANOW,&old_stdio);
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
 
	// Set up rapid I/O for terminal 
	// Opens stdout/stdin as file (faster than printf/scanf)
        struct termios stdio_ctrl;
 
        tcgetattr(STDOUT_FILENO,&old_stdio);    // Save old terminal settings
 
        memset(&stdio_ctrl,0,sizeof(stdio_ctrl));
        stdio_ctrl.c_iflag=0;
        stdio_ctrl.c_oflag=0;
        stdio_ctrl.c_cflag=0;
        stdio_ctrl.c_lflag=0;
        stdio_ctrl.c_cc[VMIN]=1;
        stdio_ctrl.c_cc[VTIME]=0;
        tcsetattr(STDOUT_FILENO,TCSANOW,&stdio_ctrl);
        tcsetattr(STDOUT_FILENO,TCSAFLUSH,&stdio_ctrl);
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);       // make the reads non-blocking


	// Open read-write, no terminal control, nonblocking 
        port_fd=open(PORT,O_RDWR | O_NOCTTY | O_NDELAY);      
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
        port_ctrl.c_cc[VMIN]=255;    // Accepts up to 255 chars in a batch
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

	char char_buf;

	printf("ROT13 Client Ready.    Port: \"%s\"\n",PORT);

        while (1) {
		 if (read(port_fd,&char_buf,1)>0)        
            		write(STDOUT_FILENO,&char_buf,1);  
        	if (read(STDIN_FILENO,&char_buf,1)>0)  
            		write(port_fd,&char_buf,1);  

	}	

        return 0;
}
