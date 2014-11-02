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

#include "test.h"
#include "decide.h"
#include <stdio.h>
#define TRUE 1
#define FALSE 0

// Takes x and y coordinates for two Cartesian points and returns the distance
// between the two points
inline double get_distance(double x1, double x2, double y1, double y2)
{
    return sqrt(pow(x1-x2,2)+pow(y1-y2,2));
}

// Takes the distance between three points and returns the angle <213
inline double get_angle(double l12, double l13, double l23)
{
return acos((pow(l12,2)+pow(l13,2)-pow(l23,2))/(2*l12*l13));
}

// Takes the x and y coordinates of two Cartesian points and returns the slope
inline double get_slope(double x1, double x2, double y1, double y2)
{
    return (y1-y2)/(x1-x2);
}
// Takes slope and y intercept of the line (y = mx + c) and returns distance of point from the line.
inline double pt_line_distance(double m,double c,double x1, double y1)
{
double z;
z = (m*x1 - y1 + c)/sqrt((pow(m,2)+1));
if (DOUBLECOMPARE(z,0)==GT||DOUBLECOMPARE(z,0)==EQ) 
return z;
else
return -1*z;
}

double get_quadrant(double x1, double y1)//Returns quadrant based on priority
                                         //ordering when a point is in conflict
{
//if (x1>=0&&y1>=0)
if(((DOUBLECOMPARE(x1,0)==GT)||(DOUBLECOMPARE(x1,0)==EQ))&&((DOUBLECOMPARE(y1,0)==GT)||(DOUBLECOMPARE(y1,0)==EQ)))
return 1;
//if (x1<0&&y1>=0)
if((DOUBLECOMPARE(x1,0)==LT)&&((DOUBLECOMPARE(y1,0)==GT)||(DOUBLECOMPARE(y1,0)==EQ)))
return 2;
//if(x1<=0&&y1<0)
if(((DOUBLECOMPARE(x1,0)==LT)||(DOUBLECOMPARE(x1,0)==EQ))&&(DOUBLECOMPARE(y1,0)==LT))
return 3;
else
return 4;
} 

//LIC_0() is checks the distance between consec. data points.
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
        //if distance exceeds LENGTH1 CMV[0] is set to true or 1 and looping is
        //terminated.

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
        if((DOUBLECOMPARE(x1,x2)==EQ)&&(DOUBLECOMPARE(x2,x3)==EQ)&&
                (DOUBLECOMPARE(y1,y2)==EQ)&&(DOUBLECOMPARE(y2,y3)==EQ))
        {
        	CMV[1]=FALSE;
        	break;
        }

        // Find the angle where these two lines intersect, if greater than 90
        // then the line made by p1 and p3 is the diameter of the circle
        double theta1 = get_angle(l12,l13,l23);
        if (DOUBLECOMPARE(theta1,PI/2)==GT) {
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
        if (DOUBLECOMPARE(theta2,PI/2)==GT) {
            // If this is greater than RADIUS1 then set the CMV and return
            if (DOUBLECOMPARE(l13/2,PARAMETERS.RADIUS1)==GT) {
                CMV[1] = TRUE;
                return;
            }
            else
                continue;
        }
        // Check the last angle
        if (DOUBLECOMPARE(theta1+theta2,PI/2)==LT) {
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
        double radius = get_distance(center_x,x1,center_y,y1);
        if (DOUBLECOMPARE(radius,PARAMETERS.RADIUS1)==GT) {
            CMV[1] = TRUE;
            return;
        }
        else
            continue;
    }
    
} // End function LIC_1()

//Checks the angle condition mentioned in specification 2.
void LIC_2() 
{
    CMV[2] = FALSE;
    // Loop through X and Y
    int i;
    for (i=0;i<NUMPOINTS-2;++i) 
    {
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
        double angle = get_angle(l12,l13,l23);
	if(DOUBLECOMPARE(angle,0.0)==EQ)
	angle = PI;
        //To include a case where any two points are the same. Check if x1==x2 && y1==y2 ||x1==x3 && y1==y3 || x2==x3 && y2==y3
        if((DOUBLECOMPARE(x1,x2)==EQ)&&(DOUBLECOMPARE(y1,y2)==EQ)||
                (DOUBLECOMPARE(x1,x3)==EQ)&&(DOUBLECOMPARE(y1,y3)==EQ)||
			(DOUBLECOMPARE(x2,x3)==EQ)&&(DOUBLECOMPARE(y2,y3)==EQ))
            break;
	if((DOUBLECOMPARE(angle,PI)==EQ))
	    break;	

	// checks for angle < (PI − EPSILON)or angle > (PI + EPSILON)
        else if((DOUBLECOMPARE(angle,(PI+PARAMETERS.EPSILON))==GT)||
                (DOUBLECOMPARE(angle,(PI-PARAMETERS.EPSILON))==LT))
        {
            CMV[2]=TRUE;
        }
    }//closes for
}//end of LIC_2()


//Funtion that checks if area formed by three consecutive data points is greater
//than AREA1 

void LIC_3()
{
    CMV[3] = FALSE;
    int i;
    // Loop through X and Y
    for (i=0;i<NUMPOINTS-2;++i) 
    {
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
        double hp = (l12+l13+l23)/2; // calculating half-perimeter
        double area = sqrt(hp*(hp-l12)*(hp-l13)*(hp-l23));
        if (DOUBLECOMPARE(area,PARAMETERS.AREA1)==GT)
        {
            CMV[3] = TRUE;
            return;
        }
    }//closes for loop
}// end of LIC3 functions LIC3 function


//Finds set of Q_PTS consecutive data points that lie in more than QUADS
//quadrants.
void LIC_4()
{
    CMV[4] = FALSE;
    int i,j,q1,q2,q3,q4;

    for(i=0;i<NUMPOINTS;i++)//NUMPOINTS
    {
        q1 = 0;
        q2 = 0;
        q3 = 0;
        q4 = 0;
        // iterate over Q_PTS consecutive points and stop if you reach the last
        // NUMPOINT
        for(j=i;(j<i+PARAMETERS.Q_PTS)&&(i+PARAMETERS.Q_PTS<NUMPOINTS+1);j++)
        {
            double z = get_quadrant(X[j],Y[j]);
            if(z == 1)
                q1++;
            else if(z == 2)
                q2++;
            else if(z == 3)
                q3++;
            else
                q4++;           
        }//closes inner for loop for Q_PTS
        switch(PARAMETERS.QUADS)
        {
            //more than 1 Quad
            case 1:
                if(!((q1==PARAMETERS.Q_PTS)||(q2==PARAMETERS.Q_PTS)||
                            (q3==PARAMETERS.Q_PTS)||(q4==PARAMETERS.Q_PTS)))
                {
                    CMV[4]=TRUE;
                    break;
                }
                //more than 2 Quad
            case 2:
                if(q1==0||q2==0||q3==0||q4==0)
                {
                    CMV[4]=TRUE;
                    break;
                }
                //more than 3 Quad
            case 3:
                if((q1*q2*q3*q4)>0)
                {
                    CMV[4]=TRUE;
                    break;
                }
        }
    }//closes for loop
}// end of LIC4 function LIC4 function

//check for the condition (X[i+1]-X[i])<0
void LIC_5()
{
    CMV[5] = FALSE;
    // Loop through X and Y
    int i;
    for (i=0;i<NUMPOINTS-1;++i)  // Get the coordinates for two points 
    {
        if(DOUBLECOMPARE(X[i+1],X[i])==LT)
        {
            CMV[5]= TRUE;
            break;
        }
    }//closes for
}//end of LIC_5()

// *****************LIC_6************************ 
void LIC_6()
{
CMV[6] = FALSE;
if(NUMPOINTS <3) //return if NUMPOINTS <3
return;
int i,j;
double m,c,d;
for(i=0;(i+PARAMETERS.N_PTS-1< NUMPOINTS);i++)
{
for(j=i+1;j<i+PARAMETERS.N_PTS-1;j++)
{ 
if((DOUBLECOMPARE(X[i],X[i+PARAMETERS.N_PTS-1])==EQ) && (DOUBLECOMPARE(Y[i],Y[i+PARAMETERS.N_PTS-1])==EQ))// if two points are coincident
{
d = get_distance(X[i],X[j],Y[i],Y[j]);
if (DOUBLECOMPARE(d,PARAMETERS.DIST)==GT) 
{
CMV[6] = TRUE;
return;
}
}//closes if statement which tests whether two points are coincident.
else
{
m = get_slope(X[i],X[i+PARAMETERS.N_PTS-1],Y[i],Y[i+PARAMETERS.N_PTS-1]);
c = Y[i]-m*X[i];
d = pt_line_distance(m,c,X[j],Y[j]);
if (DOUBLECOMPARE(d,PARAMETERS.DIST)==GT) 
{
CMV[6] = TRUE;
return;
}
}//closes else
}//closes inner for loop j
}//closes outer for loop i
}//end of LIC_6()


 //*****************LIC7()*********************

void LIC_7()
{
CMV[7]=FALSE;
if(NUMPOINTS <3) //return if NUMPOINTS <3
return;
int i;
float d;
for(i=0;(i+PARAMETERS.K_PTS+1< NUMPOINTS);i++)
{
d= get_distance(X[i],X[i+PARAMETERS.K_PTS+1],Y[i],Y[i+PARAMETERS.K_PTS+1]);
if (DOUBLECOMPARE(d,PARAMETERS.LENGTH1)==GT)
{
CMV[7]=TRUE;
return;
} 
}
}//end of LIC_7()


//*****************LIC10()*********************
void LIC_10()
{
CMV[10]=FALSE;
if(NUMPOINTS < 5)
return;
int i;
for(i=0;(i+PARAMETERS.E_PTS+PARAMETERS.F_PTS+1+1< NUMPOINTS);i++)
{
double x1 = X[i];
double x2 = X[i+PARAMETERS.E_PTS+1];
double x3 = X[i+PARAMETERS.E_PTS+1+PARAMETERS.F_PTS+1];
double y1 = Y[i];
double y2 = Y[i+PARAMETERS.E_PTS+1];
double y3 = Y[i+PARAMETERS.E_PTS+1+PARAMETERS.F_PTS+1];
double l12 = get_distance(x1,x2,y1,y2);
double l13 = get_distance(x1,x3,y1,y3);
double l23 = get_distance(x2,x3,y2,y3);
double hp = (l12+l13+l23)/2; // calculating half-perimeter
double area = sqrt(hp*(hp-l12)*(hp-l13)*(hp-l23));
if(DOUBLECOMPARE(area,PARAMETERS.AREA1)==GT)
{
CMV[10]=TRUE;
return;
}
}
}//End of LIC_10
//*************LIC_11()****************************
LIC_11()
{
CMV[11] = FALSE;
if(NUMPOINTS < 3)
return;
int i;
for(i=0;(i+PARAMETERS.G_PTS+1< NUMPOINTS);i++)  
{
if(DOUBLECOMPARE(X[i+PARAMETERS.G_PTS+1],X[i])==LT)
{
CMV[11]= TRUE;
return;
}
}
}//end of LIC11



//*********************LIC12()*************************
void LIC_12()
{
CMV[12]=FALSE;
if(NUMPOINTS <3) //return if NUMPOINTS <3
return;
int i,j;
float d;
for(i=0;(i+PARAMETERS.K_PTS+1< NUMPOINTS);i++)
{
  d= get_distance(X[i],X[i+PARAMETERS.K_PTS+1],Y[i],Y[i+PARAMETERS.K_PTS+1]);
	if (DOUBLECOMPARE(d,PARAMETERS.LENGTH1)==GT)
	{
	  for(j=0;(j+PARAMETERS.K_PTS+1< NUMPOINTS);j++)
          {
	    d= get_distance(X[j],X[j+PARAMETERS.K_PTS+1],Y[j],Y[j+PARAMETERS.K_PTS+1]);
	      if (DOUBLECOMPARE(d,PARAMETERS.LENGTH2)==LT)
	      {
		CMV[12]=TRUE;
		return;
	      }
	  } 
	}
}
}//End of LIC_12

//********************LIC_14()****************************************
void LIC_14()
{
CMV[14]=FALSE;
if(NUMPOINTS < 5)
return;
int i,j;
for(i=0;(i+PARAMETERS.E_PTS+PARAMETERS.F_PTS+1+1< NUMPOINTS);i++)
{
double x1 = X[i];
double x2 = X[i+PARAMETERS.E_PTS+1];
double x3 = X[i+PARAMETERS.E_PTS+1+PARAMETERS.F_PTS+1];
double y1 = Y[i];
double y2 = Y[i+PARAMETERS.E_PTS+1];
double y3 = Y[i+PARAMETERS.E_PTS+1+PARAMETERS.F_PTS+1];
double l12 = get_distance(x1,x2,y1,y2);
double l13 = get_distance(x1,x3,y1,y3);
double l23 = get_distance(x2,x3,y2,y3);
double hp = (l12+l13+l23)/2; // calculating half-perimeter
double area = sqrt(hp*(hp-l12)*(hp-l13)*(hp-l23));
//find if the area formed by the points is greater than PARAMETER.AREA1
if(DOUBLECOMPARE(area,PARAMETERS.AREA1)==GT)
  {
    for(j=0;(j+PARAMETERS.E_PTS+PARAMETERS.F_PTS+1+1< NUMPOINTS);j++)
      {
        double x1 = X[j];
	double x2 = X[j+PARAMETERS.E_PTS+1];
	double x3 = X[j+PARAMETERS.E_PTS+1+PARAMETERS.F_PTS+1];
	double y1 = Y[j];
	double y2 = Y[j+PARAMETERS.E_PTS+1];
	double y3 = Y[j+PARAMETERS.E_PTS+1+PARAMETERS.F_PTS+1];
	double l12 = get_distance(x1,x2,y1,y2);
	double l13 = get_distance(x1,x3,y1,y3);
	double l23 = get_distance(x2,x3,y2,y3);
	double hp = (l12+l13+l23)/2; // calculating half-perimeter
	double area = sqrt(hp*(hp-l12)*(hp-l13)*(hp-l23));
//find if the area formed by the points is less than PARAMETER.AREA2
	if(DOUBLECOMPARE(area,PARAMETERS.AREA2)==LT)
	  {       
		CMV[14]=TRUE;
		return;
	  }
	}
  }
}//closes i forloop
}//End of LIC_14




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

