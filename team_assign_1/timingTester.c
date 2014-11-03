/* Timing code inspired by http://www.guyrutenberg.com/2007/09/22/profiling-code-using-clock_gettime/
 * ECDF formula taken from Statistics Textbook
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decide.h"

#define NUM_TESTS 100000
#define PLOT_POINTS 1000

void allocateGlobals(void);
void populateGlobals(void);
struct timespec diff(struct timespec start, struct timespec end);
void generate_ECDF(struct timespec* dataSet, unsigned int dataSet_size, unsigned int numPoints);

int main()
{
    allocateGlobals();
    populateGlobals();

    // Array of timespec structs to hold output data
    struct timespec* timingData = malloc(NUM_TESTS * sizeof(struct timespec));
    
	struct timespec start, stop;
    int i;

    FILE *rawData= fopen("rawData", "w+");
    printf("Begin running timing test.\n");
    for(i=0; i < NUM_TESTS; i++) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        DECIDE();
        clock_gettime(CLOCK_MONOTONIC, &stop);
        timingData[i] = diff(start,stop);
        fprintf(rawData,"%d %ld\n",i,timingData[i].tv_nsec);
    }    
    fclose(rawData);
    printf("Done running timing test.\n");
    generate_ECDF(timingData, NUM_TESTS, PLOT_POINTS);   
    free(timingData);
    
    printf("CMV = [%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]\n",CMV[0],CMV[1],CMV[2],CMV[3],CMV[4],CMV[5],CMV[6],CMV[7],CMV[8],CMV[9],CMV[10],CMV[11],CMV[12],CMV[13],CMV[14]);
    printf("FUV = [%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]\n",FUV[0],FUV[1],FUV[2],FUV[3],FUV[4],FUV[5],FUV[6],FUV[7],FUV[8],FUV[9],FUV[10],FUV[11],FUV[12],FUV[13],FUV[14]);

    printf("Launch Missile: %s\n",(LAUNCH)? "YES": "NO");

    return 0;
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
    
    FILE *pipe = popen("gnuplot ", "w");
    fprintf(pipe, "plot sin(x)");
  // fprintf(pipe, "set title 'Empirical Cumulative Distribution Function Plot' font 'sans,18';");
   // fprintf(pipe, "set xlabel 'Time (nS)' font 'sans,14';");
   // fprintf(pipe, "set ylabel 'ECFD \%%' font 'sans,14';");
   // fprintf(pipe, "set grid; unset key;");
   // fprintf(pipe, "plot 'plotData' with lines;");
    pclose(pipe);
    
    
}

void populateGlobals() {

    PARAMETERS.LENGTH1 = 11;   // Length in LICs 0, 7, 12
    PARAMETERS.RADIUS1 = 5;   // Radius in LICs 1, 8, 13
    PARAMETERS.EPSILON = 2;   // Deviation from PI in LICs 2, 9
    PARAMETERS.AREA1 = 23 ;     // Area in LICs 3, 10, 14
    PARAMETERS.Q_PTS = 7;        // No. of consecutive points in LIC 4
    PARAMETERS.QUADS = 3;        // No. of quadrants in LIC 4
    PARAMETERS.DIST = 17;      // Distance in LIC 6 
    PARAMETERS.N_PTS = 5;        // No. of consecutive pts. in LIC 6
    PARAMETERS.K_PTS = 3;        // No. of int. pts. in LICs 7, 12
    PARAMETERS.A_PTS = 13;        // No. of int. pts. in LICs 8, 13
    PARAMETERS.B_PTS = 2;        // No. of int. pts. in LICs 8, 13
    PARAMETERS.C_PTS = 7;        // No. of int. pts. in LIC 9
    PARAMETERS.D_PTS = 5;        // No. of int. pts. in LIC 9
    PARAMETERS.E_PTS = 3;        // No. of int. pts. in LICs 10, 14
    PARAMETERS.F_PTS = 17;        // No. of int. pts. in LICs 10, 14
    PARAMETERS.G_PTS = 13;        // No. of int. pts. in LIC 11
    PARAMETERS.LENGTH2 = 31;   // Maximum length in LIC 12
    PARAMETERS.RADIUS2 = 23;   // Maximum radius in LIC 13
    PARAMETERS.AREA2 = 41;     // Maximum area in LIC 14

      double X_init[100] = {1,3,5,7,-2,-2,-2,-2,0,6,1,1,5,0,4.11,16.13,0,1,2,1,1,-1,1,-1,1,-1,1,1,0,0,3,3,-1,};
      double Y_init[100] = {2,4,6,8,-2,0,6,0,0,0,1,1,1,0,-7.37,-9.35,0,2,0,-4,2,-1,-4,-1,2,-1,-4,2,1,15,3,-1,-1,}; 

    CONNECTORS LCM_init[15][15] = {
        {ORR,ANDD,NOTUSED,ORR,NOTUSED,ANDD,NOTUSED,ORR,ANDD,ANDD,ORR,NOTUSED,ANDD,ORR,NOTUSED},
        {ANDD,ORR,NOTUSED,ANDD,NOTUSED,ORR,ANDD,ANDD,ORR,NOTUSED,ANDD,NOTUSED,NOTUSED,ORR,NOTUSED},
        {NOTUSED,ANDD,ANDD,ORR,ORR,NOTUSED,ORR,NOTUSED,ANDD,ORR,NOTUSED,ANDD,NOTUSED,ORR,NOTUSED},
        {ANDD,ORR,NOTUSED,ANDD,NOTUSED,ORR,ANDD,ANDD,ORR,NOTUSED,ANDD,NOTUSED,NOTUSED,ORR,NOTUSED},
        {NOTUSED,ANDD,ANDD,ORR,ORR,NOTUSED,ORR,NOTUSED,ANDD,ORR,NOTUSED,ANDD,NOTUSED,ORR,NOTUSED},
        {ORR,ANDD,NOTUSED,ORR,NOTUSED,ANDD,NOTUSED,ORR,ANDD,ANDD,ORR,NOTUSED,ANDD,ORR,NOTUSED},
        {ANDD,ORR,NOTUSED,ANDD,NOTUSED,ORR,ANDD,ANDD,ORR,NOTUSED,ANDD,NOTUSED,NOTUSED,ORR,NOTUSED},
        {NOTUSED,ANDD,ANDD,ORR,ORR,NOTUSED,ORR,NOTUSED,ANDD,ORR,NOTUSED,ANDD,NOTUSED,ORR,NOTUSED},
        {ORR,ANDD,NOTUSED,ORR,NOTUSED,ANDD,NOTUSED,ORR,ANDD,ANDD,ORR,NOTUSED,ANDD,ORR,NOTUSED},
        {ANDD,ORR,NOTUSED,ANDD,NOTUSED,ORR,ANDD,ANDD,ORR,NOTUSED,ANDD,NOTUSED,NOTUSED,ORR,NOTUSED},
        {NOTUSED,ANDD,ANDD,ORR,ORR,NOTUSED,ORR,NOTUSED,ANDD,ORR,NOTUSED,ANDD,NOTUSED,ORR,NOTUSED},
        {ANDD,ORR,NOTUSED,ANDD,NOTUSED,ORR,ANDD,ANDD,ORR,NOTUSED,ANDD,NOTUSED,NOTUSED,ORR,NOTUSED},
        {NOTUSED,ANDD,ANDD,ORR,ORR,NOTUSED,ORR,NOTUSED,ANDD,ORR,NOTUSED,ANDD,NOTUSED,ORR,NOTUSED},
        {ORR,ANDD,NOTUSED,ORR,NOTUSED,ANDD,NOTUSED,ORR,ANDD,ANDD,ORR,NOTUSED,ANDD,ORR,NOTUSED},
        {ANDD,ORR,NOTUSED,ANDD,NOTUSED,ORR,ANDD,ANDD,ORR,NOTUSED,ANDD,NOTUSED,NOTUSED,ORR,NOTUSED}};
    
    boolean PUM_init[15][15] = {
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}};


    int i, j;
    for(i=0; i < 15; i++) {
        CMV[i] = 0;
        FUV[i] = 0;
        for(j=0; j < 15; j++) {
            LCM[i][j] = LCM_init[i][j];
            PUM[i][j] = PUM_init[i][j];
        }
    }

    for(i=0; i < 100; i++) {
        X[i] = X_init[i];
        Y[i] = Y_init[i];
    }
}

void fail(char* msg) {
    printf("FAIL: %s",msg);
    exit(1);
}

void allocateGlobals() {
    int i;

    // Allocate X coordinates of data points
    X = (double*) malloc (100 * sizeof (double));
    if (!X) fail("X Alloc");
    
    // Allocate Y coordinates of data points
    Y = (double*) malloc (100 * sizeof (double));
    if (!Y) fail("Y Alloc");

    // Allocate Conditions Met Vector
    CMV = (boolean*) malloc (15 * sizeof (boolean));
    if (!CMV) fail("CMV Alloc");

    // Allocate Final Unlocking Vector
    FUV = (boolean*) malloc (15 * sizeof (boolean));
    if (!FUV) fail("FUV Alloc");

    // Allocate Logical Connector Matrix    
    LCM = (CONNECTORS **) malloc (15 * sizeof (CONNECTORS*));
    if (!LCM) fail("LCM Outer Alloc");
    
    for (i=0; i<15; i++) {
        LCM[i] = (CONNECTORS *) malloc (15 * sizeof (CONNECTORS));
        if (!LCM[i]) fail("LCM Inner Alloc");
    }
    
    // Allocate Preliminary Unlocking Matrix
    PUM = (boolean **) malloc (15 * sizeof (boolean*));
    if (!PUM) fail("PUM Outer Alloc");
    
    for (i=0; i<15; i++) {
        PUM[i] = (boolean *) malloc (15 * sizeof (boolean));
        if (!PUM[i]) fail("PUM Inner Alloc");
    }
}

