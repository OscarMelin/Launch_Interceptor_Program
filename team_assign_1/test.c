
#include"test.h"
#include"decide.h"
#include<minunit.h>
#include<stdlib.h> // malloc()

MU_TEST(test_distance)
{
    mu_assert_double_eq(get_distance(2,2,2,2),0);
    mu_assert_double_eq(get_distance(1,2,0,0),1);
    mu_assert_double_eq(get_distance(1,-1,0,0),2);
    mu_assert_double_eq(get_distance(2,2,-2,1),3);
    mu_assert_double_eq(get_distance(-2,2,-2,2),5.6568);
}

MU_TEST(test_angle)
{
    mu_assert_double_eq(get_angle(2,2,2),PI/3);
    mu_assert_double_eq(get_angle(3,4,5),PI/2);
    mu_assert_double_eq(get_angle(3,4,6),2.04691539);
    mu_assert_double_eq(get_angle(4,6,3),0.4604934);
    mu_assert_double_eq(get_angle(3,6,4),0.634183263);
}

MU_TEST(test_slope)
{
    double result = get_slope(4,4,1,7);
    mu_check(isinf(result)==-1);
    result = get_slope(2,2,3,0);
    mu_check(isinf(result)==1);
    mu_assert_double_eq(get_slope(-2,4,-3,-3),0);
    mu_assert_double_eq(get_slope(1,3,1,5),2);
    mu_assert_double_eq(get_slope(0,-3,0,-3),1);
}

void test_setup()
{
    NUMPOINTS = 3;
    X = malloc(NUMPOINTS*sizeof(double));
    Y = malloc(NUMPOINTS*sizeof(double));
    CMV = malloc(15*sizeof(boolean));
}

void test_teardown()
{
    free(X);
    free(Y);
    free(CMV);
}

MU_TEST(test_samepoint_LIC_1)
{
    X[0] = 3;
    X[1] = 3;
    X[2] = 3;
    Y[0] = -1;
    Y[1] = -1;
    Y[2] = -1;
    PARAMETERS.RADIUS1 = 0;

    LIC_1();
    mu_check(CMV[1]==0);

    PARAMETERS.RADIUS1 = 1;
    LIC_1();
    mu_check(CMV[1]==0);
}

MU_TEST(test_less_than_3_points_LIC_1)
{
    NUMPOINTS = 2;
    X[0] = 0;
    X[1] = 1;
    Y[0] = 0;
    Y[1] = 15;
    PARAMETERS.RADIUS1 = 0;

    LIC_1();
    mu_check(CMV[1]==0);

    PARAMETERS.RADIUS1 = 3;
    mu_check(CMV[1]==0);
}

MU_TEST(test_obtuse_triangle_LIC_1)
{
    X[0] = -1;
    Y[0] = -1;
    X[1] = 1;
    Y[1] = -4;
    X[2] = 1;
    Y[2] = 2;
    PARAMETERS.RADIUS1 = 2.9;

    LIC_1();
    mu_check(CMV[1]==1);
    
    PARAMETERS.RADIUS1 = 3;
    LIC_1();
    mu_check(CMV[1]==0);
}

MU_TEST(test_second_angle_obtuse)
{
    X[0] = 1;
    Y[0] = -4;
    X[1] = -1;
    Y[1] = -1;
    X[2] = 1;
    Y[2] = 2;
    PARAMETERS.RADIUS1 = 2.9;

    LIC_1();
    mu_check(CMV[1]==1);
    
    PARAMETERS.RADIUS1 = 3;
    LIC_1();
    mu_check(CMV[1]==0);
}

MU_TEST(test_third_angle_obtuse)
{
    X[0] = 1;
    Y[0] = -4;
    X[1] = 1;
    Y[1] = 2;
    X[2] = -1;
    Y[2] = -1;
    PARAMETERS.RADIUS1 = 2.9;

    LIC_1();
    mu_check(CMV[1]==1);
    
    PARAMETERS.RADIUS1 = 3;
    LIC_1();
    mu_check(CMV[1]==0);
}    

MU_TEST(test_triangle_all_angles_acute)
{
    X[0] = 0;
    Y[0] = 0;
    X[1] = 1;
    Y[1] = 2;
    X[2] = 2;
    Y[2] = 0;
    PARAMETERS.RADIUS1 = 1;

    LIC_1();
    mu_check(CMV[1]==1);
    
    PARAMETERS.RADIUS1 = 2;
    LIC_1();
    mu_check(CMV[1]==0);
}

MU_TEST(test1)
{
    X[0] = 0;
    Y[0] = 0;
    X[1] = 4.11;
    Y[1] = -7.37;
    X[2] = 16.13;
    Y[2] = -9.35;
    PARAMETERS.RADIUS1 = 13;
    LIC_1();
    mu_check(CMV[1]==0);
}

MU_TEST_SUITE(test_suite_LIC_1)
{
    MU_SUITE_CONFIGURE(&test_setup,&test_teardown);
    MU_RUN_TEST(test_samepoint_LIC_1);
    MU_RUN_TEST(test_less_than_3_points_LIC_1);
    MU_RUN_TEST(test_obtuse_triangle_LIC_1);
    MU_RUN_TEST(test_second_angle_obtuse);
    MU_RUN_TEST(test_third_angle_obtuse);
    MU_RUN_TEST(test_triangle_all_angles_acute);
    MU_RUN_TEST(test1);
}

int main()
{
    MU_RUN_TEST(test_distance);
    MU_RUN_TEST(test_angle);
    MU_RUN_TEST(test_slope);
    MU_RUN_SUITE(test_suite_LIC_1);
    MU_REPORT();
    return 0;
}

