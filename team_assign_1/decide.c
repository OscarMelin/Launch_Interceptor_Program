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


void LIC_0() {

} // End function LIC_0()


void LIC_1() {

} // End function LIC_1()


void DECIDE(void) {

    // Call launch interceptor condition functions
    LIC_0();
    LIC_1();

    // Loop through and populate the PUM
    for(int row = 0; row < 15; row++) {
        for(int col = 0; col < 15; col++) {
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
    for(int row = 0; row < 15; row++) {
        //Test diagonal elements of the PUM
        if(PUM[row][row] == FALSE ) {
            FUV[row] = TRUE;
            continue; 
        }
        
        char flag = TRUE; 
        for(int col = 0; col < 15; col++) {
            if(PUM[row][col] == FALSE) {
                flag = FALSE;
            }
        }
        FUV[row] = flag;
    }

    // Set the final launch enable if the FUV elements are all TRUE
    LAUNCH = TRUE;
    for(int row = 0; row < 15; row++) {
        if(FUV[row] == FALSE) {
            LAUNCH = FALSE;
        }
    }

} // End function DECIDE()


 
