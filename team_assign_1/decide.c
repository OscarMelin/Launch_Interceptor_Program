/* CS 5785/6785
 * Team Assignment #1
 *
 * Brent Mellor
 * Shakti Chaganty 
 * Eric Hair
 *
 * decide.c
 *    Part of a hypothetical anti-ballistic missile system.
 *    Determines whether an interceptor should be launched 
 *    based upon input radar tracking information.
 */

#include "decide.h"
#define TRUE 1
#define FALSE 0

double get_distance(double x1, double x2, double y1, double y2)
{
    return sqrt(pow(x1-x2,2)+pow(y1-y2,2));
}

double get_angle(double l12, double l13, double l23)
{
    return acos((pow(l12,2)+pow(l13,2)-pow(l23,2))/(2*l12*l13));
}

double get_slope(double x1, double x2, double y1, double y2)
{
    return (y1-y2)/(x1-x2);
}

//LIC_() is checks the distance between consec. data points.
//If the distance between any two datapoints is grater than
//LENGTH1, writes 1 into CMV[0].

void LIC_0() 
{
	int i = 0;
	int flag = 0;
	double distance;

	//loop through i data points.

	while (i<NUMPOINTS-1 && flag ==0)
	{
		// Get the coordinates for the two consecutive points

		double x1 = X[i];
		double x2 = X[i + 1];
		double y1 = Y[i];
		double y2 = Y[i + 1];

		distance = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
		// Calculate the distance between each set of consecutive data points.
		//if distance exceeds LENGTH1 CMV[0] is set to true or 1 and looping is terminated.

		if (DOUBLECOMPARE(distance, PARAMETERS.LENGTH1) == GT)
		{
			flag = 1;
			CMV[0] = TRUE;
			break;
		}
		else
		{
			CMV[0] = FALSE;
		}
		i++;
	}
} // End function LIC_0()

/*
 * The LIC_1 function checks three consecutive data points (p1, p2, p3) to see
 * if all three can be contained in a circle of radius RADIUS1
 */
void LIC_1() {

    CMV[1] = FALSE;

    // Loop through X and Y
    for (int i=0;i<NUMPOINTS-2;++i) {

        // Get the coordinates for the three points
        double x1 = X[i];
        double x2 = X[i+1];
        double x3 = X[i+2];
        double y1 = Y[i];
        double y2 = Y[i+1];
        double y3 = Y[i+2];
        double l12 = get_distance(x1,x2,y1,y2);
        double l13 = get_distance(x1,x3,y1,y3);
        double l23 = get_distance(x2,x3,y2,y3);
        
        //To include a case where all the three points are the same.
        if((x1 == x2 == x3)&&(y1 == y2 == y3))
        {
        	CMV[1]=FALSE;
        	break;
        }

        // Find the angle where these two lines intersect, if greater than 90
        // then the line made by p1 and p3 is the diameter of the circle
        double theta1 = get_angle(l12,l13,l23);
        if (theta1 > PI/2) {
            // If this is greater than RADIUS1 then set the CMV and return
            if (DOUBLECOMPARE(l23/2,PARAMETERS.RADIUS1)==GT) {
                CMV[1] = TRUE;
                return;
            }
            else
                continue;
        }
        // If the angle is <= 90, then check one more angle
        double theta2 = get_angle(l12,l23,l13);
        if (theta2 > PI/2) {
            // If this is greater than RADIUS1 then set the CMV and return
            if (DOUBLECOMPARE(l13/2,PARAMETERS.RADIUS1)==GT) {
                CMV[1] = TRUE;
                return;
            }
            else
                continue;
        }
        // Check the last angle
        if (theta1+theta2 < PI/2) {
            if (DOUBLECOMPARE(l12/2,PARAMETERS.RADIUS1)==GT) {
                CMV[1] = TRUE;
                return;
            }
            else
                continue;
        }
        // Now calculate the center of the circle
        double m_a = get_slope(x1,x2,y1,y2);
        double m_b = get_slope(x2,x3,y2,y3);
        double center_x = (m_a*m_b*(y1-y3)+m_b*(x1+x2)-m_a*(x2+x3))
            /(2*(m_b-m_a));
        double center_y = -1/m_a*(center_x-(x1+x2)/2)+(y1+y2)/2;
        // All three points lie on the circle, so calculate the distance from
        // here out
        double radius = sqrt(pow(center_x-x1,2)+pow(center_y-y1,2));
        if (DOUBLECOMPARE(radius,PARAMETERS.RADIUS1)==GT) {
            CMV[1] = TRUE;
            return;
        }
        else
            continue;
    }
    
} // End function LIC_1()


void DECIDE(void) {

    // Call launch interceptor condition functions
    LIC_0();
    LIC_1();

    // Loop through and populate the PUM
    int row, col;
    for(row = 0; row < 15; row++) {
        for(col = 0; col < 15; col++) {
            //Skip diagonal elements of the PUM, they are inputs.
            if(row == col) {
                continue;
            }
            // Check conditions in the LCM and CMV to populate the PUM
            switch(LCM[row][col]) {
                case NOTUSED:
                    PUM[row][col] = TRUE;
                    break;
                case ANDD:
                    if(CMV[row] && CMV[col]) {
                        PUM[row][col] = TRUE;
                    } else {
                        PUM[row][col] = FALSE;
                    }
                    break;
                case ORR:
                    if(CMV[row] || CMV[col]) {
                        PUM[row][col] = TRUE;
                    } else {
                        PUM[row][col] = FALSE;
                    }
                    break;
            } // End switch
        } // End col 'for' loop
    } // End row 'for' loop

    /* Populate the FUV from the PUM
     * If the PUM diagonal element is FALSE, automatically set FUV element
     * Otherwise set only if entire PUM row is TRUE 
     */
    for(row = 0; row < 15; row++) {
        //Test diagonal elements of the PUM
        if(PUM[row][row] == FALSE ) {
            FUV[row] = TRUE;
            continue; 
        }
        
        boolean flag = TRUE; 
        for(col = 0; col < 15; col++) {
            if(PUM[row][col] == FALSE) {
                flag = FALSE;
            }
        }
        FUV[row] = flag;
    }

    // Set the final launch enable if the FUV elements are all TRUE
    LAUNCH = TRUE;
    for(row = 0; row < 15; row++) {
        if(FUV[row] == FALSE) {
            LAUNCH = FALSE;
        }
    }

} // End function DECIDE()


 
