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

//LIC_() is checks the distance between consec. data points.
//If the distance between any two datapoints is grater than
//LENGTH1, writes 1 into CMV[0].

void LIC_0() {
{
	int CMV[1] = { 0 };
	int i = 0;
	int flag = 0;
	double distance;

	//loop through i data points.

	while (i<NUMPOINTS && flag ==0)
	{
		// Get the coordinates for the two consecutive points

		double x1 = X[i];
		double x2 = X[i + 1];
		double y1 = Y[i];
		double y2 = Y[i + 1];

		distance = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
		// Calculate the distance between each set of consecutive data points.
		//if distance exceeds LENGTH1 CMV[0] is set to true or 1 and looping is terminated.

		if (distance >  LENGTH1)
		{
			flag = 1;
			CMV[0] = 1;
			break;
		}
		else
		{
			CMV[0] = 1;
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
    for (int i=0;i<NUMPOINTS;++i) {

        // Get the coordinates for the three points
        double x1 = X[i];
        double x2 = X[i+1];
        double x3 = X[i+2];
        double y1 = Y[i];
        double y2 = Y[i+1];
        double y3 = Y[i+2];

        // Find slopes to lines that pass through p1 and p2, and p2 and p3
        double m_a = (y2-y1)/(x2-x1);
        double m_b = (y3-y2)/(x3-x2);

        // Find the angle where these two lines intersect, if greater than 90
        // then the line made by p1 and p3 is the diameter of the circle
        double theta1;
        double theta2;
        if (DOUBLECOMPARE(m_a,m_b)==GT) {
            theta1 = atan((m_a-m_b)/(1+m_a*m_b));
        }
        else {
            theta1 = atan((m_b-m_a)/(1+m_a*m_b));
        }
        if (theta1 > PI/2) {
            // Calculate the distance between p1 and p3, this is the diameter
            double radius = sqrt(pow(x3-x1,2)+pow(y3-y1,2))/2;
            // If this is greater than RADIUS1 then set the CMV and return
            if (DOUBLECOMPARE(radius,PARAMETERS.RADIUS1)==GT) {
                CMV[1] = TRUE;
                return;
            }
        }
        // If the angle is <= 90, then check one more angle
        m_b = (y3-y1)/(x3-x1);
        if (DOUBLECOMPARE(m_a,m_b)==GT) {
            theta2 = atan((m_a-m_b)/(1+m_a*m_b));
        }
        else {
            theta2 = atan((m_b-m_a)/(1+m_a*m_b));
        }
        if (theta2 > PI/2) {
            // Calculate the distance between p1 and p3, this is the diameter
            double radius = sqrt(pow(x2-x1,2)+pow(y2-y1,2))/2;
            // If this is greater than RADIUS1 then set the CMV and return
            if (DOUBLECOMPARE(radius,PARAMETERS.RADIUS1)==GT) {
                CMV[1] = TRUE;
                return;
            }
        }
        // Check the last angle
        if (theta1+theta2 < PI/2) {
            double radius = sqrt(pow(x3-x2,2)+pow(y3-y2,2))/2;
            if (DOUBLECOMPARE(radius,PARAMETERS.RADIUS1)==GT) {
                CMV[1] = TRUE;
                return;
            }
        }
        // Now calculate the center of the circle
        double center_x = (m_a*m_b*(y3-y2)+m_a*(x1+x3)-m_b*(x1+x2))
            /(2*(m_a-m_b));
        double center_y = (-1/2*m_b*(y1+y3)+1/2*m_a*(y1+y2)+x2-x3)/(m_a-m_b);
        // All three points lie on the circle, so calculate the distance from
        // here out
        double radius = sqrt(pow(center_x-x1,2)+pow(center_y-y1,2));
        if (DOUBLECOMPARE(radius,PARAMETERS.RADIUS1)==GT)
            CMV[1] = TRUE;
        return;
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
                    if(CMV[row] && CMV[col]) {
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


 
