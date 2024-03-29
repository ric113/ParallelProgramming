/**********************************************************************
 * DESCRIPTION:
 *   Parallel by CUDA Wave Equation - C Version
 *   This program implements the concurrent wave equation
 *********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAXPOINTS 1000000
#define MAXSTEPS 1000000
#define MINPOINTS 20
#define PI 3.14159265

#define MAX_BLOCK_SIZE 1000

void check_param(void);
void printfinal (void);

int nsteps,                 	/* number of time steps */
    tpoints; 	     		/* total points along string */
float  values[MAXPOINTS+2]; 	/* values at time t */


/**********************************************************************
 *	Checks input values from parameters
 *********************************************************************/
void check_param(void)
{
   char tchar[20];

   /* check number of points, number of iterations */
   while ((tpoints < MINPOINTS) || (tpoints > MAXPOINTS)) {
      printf("Enter number of points along vibrating string [%d-%d]: "
           ,MINPOINTS, MAXPOINTS);
      scanf("%s", tchar);
      tpoints = atoi(tchar);
      if ((tpoints < MINPOINTS) || (tpoints > MAXPOINTS))
         printf("Invalid. Please enter value between %d and %d\n", 
                 MINPOINTS, MAXPOINTS);
   }
   while ((nsteps < 1) || (nsteps > MAXSTEPS)) {
      printf("Enter number of time steps [1-%d]: ", MAXSTEPS);
      scanf("%s", tchar);
      nsteps = atoi(tchar);
      if ((nsteps < 1) || (nsteps > MAXSTEPS))
         printf("Invalid. Please enter value between 1 and %d\n", MAXSTEPS);
   }

   printf("Using points = %d, steps = %d\n", tpoints, nsteps);

}

/**********************************************************************
 *     Print final results
 *********************************************************************/
void printfinal()
{
   int i;

   for (i = 1; i <= tpoints; i++) {
      printf("%6.4f ", values[i]);
      if (i%10 == 0)
         printf("\n");
   }
}
__global__
void kernelWork(float* value_d, int nsteps, int tpoints)
{
        int index = blockIdx.x * 1000 + threadIdx.x;
        float oldvalue, value, newvalue;

        // init .
        float x, fac, k, tmp;

        /* Calculate initial values based on sine curve */
        fac = 2.0 * PI;
        k = index;
        tmp = tpoints - 1;
        x = (float) k / tmp;

        value = sin (fac * x);
        oldvalue = value;

        // update .
        int i;
        float dtime, c, dx, tau, sqtau;

        /* Update values for each time step */
        for (i = 1; i<= nsteps; i++) {
                /* Update points along line for this time step */
                /* global endpoints */
                if ((index  == 0) || (index  == tpoints-1))
                        newvalue = 0.0;
                else {
                        dtime = 0.3;
                        c = 1.0;
                        dx = 1.0;
                        tau = (c * dtime / dx);
                        sqtau = tau * tau;
                        newvalue = (2.0 * value) - oldvalue + (sqtau *  (-2.0)*value);
                }
                /* Update old values with new values */
                oldvalue = value;
                value = newvalue;

        }

        value_d[index+1] = value;

}

void cudaProcess()
{
        float *value_d;

        int float_arr_size = (MAXPOINTS + 2) * sizeof(float);

        cudaMalloc( &value_d, float_arr_size );
        cudaMemcpy( value_d, values, float_arr_size, cudaMemcpyHostToDevice );

        int blockNum = ceil((float)tpoints / MAX_BLOCK_SIZE);

        kernelWork<<<blockNum, MAX_BLOCK_SIZE>>>(value_d, nsteps, tpoints);

        cudaMemcpy( values, value_d, float_arr_size, cudaMemcpyDeviceToHost );
        cudaFree( value_d );
}

/**********************************************************************
 *	Main program
 *********************************************************************/
int main(int argc, char *argv[])
{
	sscanf(argv[1],"%d",&tpoints);
	sscanf(argv[2],"%d",&nsteps);
	check_param();
	printf("Initializing points on the line...\n");
	printf("Updating all points for all time steps...\n");
  cudaProcess();
	printf("Printing final results...\n");
	printfinal();
	printf("\nDone.\n\n");
	
	return 0;
}