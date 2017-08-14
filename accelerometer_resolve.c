#include <stdio.h>
#include <stdint.h>

/**
 * This uses Q15.16 signed fixed point math.  Long story short, all that means
 * is that all of the numbers in here should be multiplied by 65536 ( =2^16) to
 * get their "real" value.
 */

/**
 * Q15.16
 */

#define ROOT2OVER2Q15_16 46341
#define ONE_Q15_16 65536
#define Q15_16_TO_DOUBLE(x) (((double)x) / (65536.0))

/**
 * 0, 120, -120
 */
const int32_t rotationMatricies[3][3][3] =
{
    {{                0,                0,       ONE_Q15_16},
     {-ROOT2OVER2Q15_16, ROOT2OVER2Q15_16,                0},
     {-ROOT2OVER2Q15_16,-ROOT2OVER2Q15_16,                0}},

    {{ ROOT2OVER2Q15_16, ROOT2OVER2Q15_16,                0},
     {                0,                0,      -ONE_Q15_16},
     {-ROOT2OVER2Q15_16, ROOT2OVER2Q15_16,                0}},

    {{ ROOT2OVER2Q15_16,-ROOT2OVER2Q15_16,                0},
     {-ROOT2OVER2Q15_16,-ROOT2OVER2Q15_16,                0},
     {                0,                0,      -ONE_Q15_16}}
};

typedef enum plane {plane0123, plane0425, plane1453, planeNone} plane_e;

/**
 * Multiplies two Q15.16 matricies.  R should be 3x3 and V should be should
 * be 1x3.
 */
void apply_3x3_mult(const int32_t* R, const int32_t* V, int32_t* target)
{
    for(int i = 0; i < 3; i++)
    {
        target[i] = 0;
        for(int j = 0; j < 3; j++)
            target[i] += (V[j] * R[j + (3 * i)]) / (65536);
    }
}

int32_t vector_distance_squared(const int32_t* a, const int32_t* b)
{
    int32_t accum = 0;
    for(int i = 0; i < 3; i++)
        accum += ((a[i] - b[i]) * (a[i] - b[i])) / (65536);

    return accum;
}

/**
 * We expect raw, signed 14-bit accelerometer readings
 */
plane_e find_likely_plane(int32_t* frameAccel, int32_t* coreAccel)
{
    int32_t transformed[3][3];
    int32_t distance[3];

    //test each of the rotation matricies.  Store all results for debug purposes.
    for(int i = 0; i < 3; i++)
    {
        apply_3x3_mult(&rotationMatricies[i][0][0], coreAccel, &transformed[i][0]);
        distance[i] = vector_distance_squared(&transformed[i][0], frameAccel);
        printf("matrix %i = %lf, %lf, %lf, distance = %lf\r\n", i,
               Q15_16_TO_DOUBLE(transformed[i][0]), Q15_16_TO_DOUBLE(transformed[i][1]),
               Q15_16_TO_DOUBLE(transformed[i][2]), Q15_16_TO_DOUBLE(distance[i]));
    }
}


int32_t core_test_matricies[][3] =
{
//    { 32767,      0,      0},
//    {     0,  32767,      0},
//    {     0,      0,  32767},
    {   344,    132,  15652},  //Angle = 0
    { 11888,  11512,  -1196},  //Angle = 120
    { 11680, -11456,  -1156},  //Angle = -120
    {   276,    208,  15524}   //Angle = 0
};

int32_t frame_test_matricies[][3] =
{
    {  16604,   292,   -296},
    {  16604,   292,   -296},
    {  16604,   292,   -296},
    {  16604,   292,   -296}
};

int main(int argc, char** argv)
{


    for(int i = 0; i < 4; i++)
    {
        printf("--------------------------------------------------\r\n");
        printf("core =   [%06i, %06i, %06i]\r\n", core_test_matricies[i][0],
               core_test_matricies[i][1], core_test_matricies[i][2]);
        printf("frame =  [%06i, %06i, %06i]\r\n", frame_test_matricies[i][0],
               frame_test_matricies[i][1], frame_test_matricies[i][2]);
        find_likely_plane(&frame_test_matricies[i][0], &core_test_matricies[i][0]);
    }

    printf("\r\n");
    return 0;
}
