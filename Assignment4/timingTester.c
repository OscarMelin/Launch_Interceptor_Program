/* Timing code inspired by http://www.guyrutenberg.com/2007/09/22/profiling-code-using-clock_gettime/
 * ECDF formula taken from Statistics Textbook
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <errno.h>

#define BAUD B115200
#define PORT "/dev/ttyO1"

#define NUM_TESTS 1000
#define PLOT_POINTS 500


struct timespec diff(struct timespec start, struct timespec end);
void generate_ECDF(struct timespec* dataSet, unsigned int dataSet_size, unsigned int numPoints);
void sig_handler(int signo);

int setupSerial(void);
void testRot13Server(void);

int port_fd;

int main()
{
    if(setupSerial()) {
        printf("Error opening serial system!\n");
        return -1;
    }
    
    // Array of timespec structs to hold output data
    struct timespec* timingData = malloc(NUM_TESTS * sizeof(struct timespec));
    
	struct timespec start, stop;
    int i;

    FILE *rawData= fopen("rawData", "w+");
    printf("Begin running timing test.\n");
    for(i=0; i < NUM_TESTS; i++) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        testRot13Server();
        clock_gettime(CLOCK_MONOTONIC, &stop);
        timingData[i] = diff(start,stop);
        fprintf(rawData,"%d %ld\n",i,timingData[i].tv_nsec);
        usleep(100000);
    }    
    fclose(rawData);
    printf("Done running timing test.\n");
    generate_ECDF(timingData, NUM_TESTS, PLOT_POINTS);   
    free(timingData);
    
	return 0;
}

int setupSerial() {
    	if (signal(SIGINT, sig_handler) == SIG_ERR) {
		printf("ERROR: Setting up signal handler!\n");
		return 1;
	}

	// Open read-write, no terminal control, blocking 
    port_fd=open(PORT,O_RDWR | O_NOCTTY);      
	if(port_fd  < 0) {
		printf("ERROR: Unable to open \"%s\"!\n",PORT);
		return 1;
	}

	if(!isatty(port_fd)) {
		printf("ERROR: \"%s\" is not an avaliable port!\n",PORT);
		return 1;
	}

	// Set for no pre-processing and recieve each character, no waiting 
	// for newline characters.
    struct termios port_ctrl;
    memset(&port_ctrl,0,sizeof(port_ctrl));
    port_ctrl.c_cflag = CRTSCTS | CS8 | CLOCAL | CREAD;
    port_ctrl.c_iflag = IGNPAR;
    port_ctrl.c_oflag = 0;
    port_ctrl.c_lflag = 0;       //ICANON;
    port_ctrl.c_cc[VMIN]=42;
    port_ctrl.c_cc[VTIME]=1;     // Read timeout of 1 sec before returning
    tcflush(port_fd, TCIFLUSH);
    tcsetattr(port_fd,TCSANOW,&port_ctrl);	


	// Set I/O baud rates
	if(cfsetispeed(&port_ctrl, BAUD) < 0 || cfsetospeed(&port_ctrl, BAUD) < 0) {
		printf("ERROR: Unable to set the baudrate!\n"); 
		return 1;
	}
	
	// Assign config to port, flush all buffers
        if(tcsetattr(port_fd,TCSAFLUSH,&port_ctrl) < 0) {
		printf("ERROR: Unable to assign configuration to port!\n");
		return 1;
	}

    return 0;
}

void testRot13Server() {
    char testStr[43] = "Foo Bar Baz, This is a timing test string!";
    char char_buf[100];
    write(port_fd,&testStr,43);
    // Blocking read until string returns
    int count = read(port_fd,&char_buf,43);
    if(errno)
         printf("ERRNO: %d\n",errno);
    //else
        //printf("Data: %s\n",char_buf);
    //printf("Number of bytes received: %d\n",count);
}

void sig_handler(int signo) {
	if (signo == SIGINT) {
		printf("Closing down port...\n");
        	close(port_fd);
		exit(0);
	}
}

struct timespec diff(struct timespec start, struct timespec end)
{
	struct timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}


void generate_ECDF(struct timespec* dataSet, unsigned int dataSet_size, unsigned int numPoints) {
    
    unsigned int i, j, sum;
    long x_coord;
    float y_coord;
    
    // Find maximum value in dataSet (only dealing with nanoseconds)
    long max = dataSet[0].tv_nsec;
    for(i=1; i < dataSet_size; i++) {
       if(dataSet[i].tv_nsec > max) 
            max = dataSet[i].tv_nsec;
    }
    printf("max value: %ld",max); 
    FILE *plotData = fopen("plotData", "w+");
    
    // generate ECDF
    for(i=0; i < numPoints; i++) {
        // Calculate x_coord
        x_coord = (max/numPoints)*i;
        
        // Calculate y_coord
        for(j=0, sum=0; j < dataSet_size; j++) {
            if(dataSet[j].tv_nsec <= x_coord)
                sum++;
        }
        y_coord = ((float)sum/numPoints);
        
        fprintf(plotData,"%ld %f\n",x_coord,y_coord);
        //printf("%ld %f\n",x_coord,y_coord);
    }
    fclose(plotData);
   
}

