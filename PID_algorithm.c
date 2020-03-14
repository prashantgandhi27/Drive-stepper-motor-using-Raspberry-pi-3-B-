/*
 * CMPE 242
 * Assignment   : C code for PID controller with LoG convolution
 * Name         : Prashant Shushilbhai Gandhi
 * STDID        : 013712361
*/
#include <stdio.h>
#include <math.h>

#define ARRAY_LEN   50    //define array length
#define KERNEL_LEN  3    //define kernel length
#define ERROR_LEN   50    //define error length

#define PI          3.1415926

#define microStepFull   1.8    //full
#define microStepHalf   0.9    //half
#define microStepQ      0.45   //quarter
#define microStep8      0.225  //one eighth

int counter = 0;

void  getDerive(float error0, float error2, float c_difference)     //central difference
{

    //printf( "sub: derivative error0= %4f error2= %4f \n", error0, error2);
    c_difference = (error2 - error0)/2; // central difference
    //printf( "sub: central difference %.4f \n\n", c_difference);

}

void  getIntegral(float *Error, int numTime, float iDError )     //integral
{
    for (int i=0; i <= numTime; i++)
    {
        iDError = iDError + Error[i]*Error[i];
    }
    //printf( "sub: integral error %4f \n", iDError);
}

float Laplace_of_gussian(float x, float* sigma)
{
    float x_square = x*x;
    float sigma_multiply_two = 2.0*(*sigma)*(*sigma);
    float sigma_five_times = (*sigma)*(*sigma)*(*sigma)*(*sigma)*(*sigma);
    float exponential_result = exp(-(x_square/sigma_multiply_two));
    float x_square_minus_sigma = x_square - sigma_multiply_two;
    float square_root_of_twopi = sqrt(2*PI);
    float twopi_sigmafivetimes = square_root_of_twopi*sigma_five_times;
    float result_of_division = x_square_minus_sigma/twopi_sigmafivetimes;
    float final_result = result_of_division*exponential_result;
    //printf("Final result = %f\n", final_result);

    return (float)final_result;
}

void convolution(const float *sigma, const float *sum, int kernel, float *output_data, int sum_size)
{
    float LoG_output[(kernel/2)+1];
    float LoG_prime[(kernel/2)+1];
    LoG_prime[0] = 0;

    for(int i=0; i<((kernel+1)/2); i++)
    {
        LoG_output[i]= Laplace_of_gussian((float)i, sigma);
    }

    for(int i=1; i<=((kernel+1)/2)-1; i++)
    {
        LoG_prime[0] = 0;
        LoG_prime[i]= LoG_output[i];
    }

    for(int i=1; i<=sum_size; i++)
    {
        if(sum[i] != 0)
        {
            output_data[i] = LoG_prime[1]*sum[i-1] + LoG_output[0]*sum[i] + LoG_output[1]*sum[i+1];
            counter++;
        }else{
            break;
        }
    }
}

int main(void)
{
    float x[ARRAY_LEN];       // Reference input
    float Error[ARRAY_LEN];   // error for P
    float iDError = 0;        // error for I
    float centralDError = 0;      // errors for D (central difference)
    float bufferError, errorPrevious;
    float output_data_of_convolution[ARRAY_LEN];
    float sumPID[ARRAY_LEN] = {0.0};

    int integralBound = 4;

    float K_p = 1;
    float K_i = 2;
    float K_d = 4;


    float sensor_reading = 30.0;
    float current_position = 0.0;
    float target_position = 0.0;
    float sigma = 0.0;
    int iindex = 0;
    int upperBound=10;               //total number of loops for control action

    printf("Enter sensor position between 51 to 100 : ");
    scanf("%f", &sensor_reading);
    printf("Enter target position between 1 to 50 : ");
    scanf("%f", &target_position);
    printf("Enter the value of sigma: ");
    scanf("%f", &sigma);

    for(iindex = 0; iindex < 3; iindex++)
    {
        current_position = sensor_reading;
        Error[iindex] = target_position - current_position;
        if(iindex == 2)
        {
            bufferError = Error[iindex];
            getDerive( Error[iindex-2], bufferError, centralDError );        //central difference
        }

        iDError = 0;                                                        //at inital condition or near initial condition, set it 0
        sumPID[iindex] = K_p * Error[iindex] + K_i * iDError + K_d * centralDError;

        sensor_reading -= 2;                                                //Decrement sensor value to reduce error
    }

    for(iindex = 3; 0 != Error[iindex - 1] ; iindex++)
    {
        current_position = sensor_reading;
        Error[iindex] = target_position - current_position;

        bufferError = Error[iindex];
        getDerive( Error[iindex-2], bufferError, centralDError );    //central difference

        if(iindex >= integralBound )
        {
            getIntegral(&Error, integralBound, iDError );    //central difference
            //printf( "Main: Integral Error = %4f\n", iDError);
        }

        sumPID[iindex]= K_p * Error[iindex] + K_i * iDError + K_d * centralDError;
        sensor_reading -= 2;
        //printf( "error= %4f, sumPID = %f\n", Error[iindex], sumPID);
    }

    convolution(&sigma, sumPID, KERNEL_LEN, output_data_of_convolution, sizeof(sumPID));

    for(int i=1; i<=counter+1; i++)
    {
        printf("convolution of sumPID = %f\n", output_data_of_convolution[i]);
    }
    return 0;
}
