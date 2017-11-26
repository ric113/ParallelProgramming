#include <stdio.h>
#include <math.h>
#include <mpi.h>

#define PI 3.1415926535

double calculate(long long int start, long long int end, double rect_width) {
    double sum = 0, x_middle, area;
    long long int i;
    for(i = start ; i <= end ; i ++) {
         x_middle = (i - 0.5) * rect_width;
          area = sin(x_middle) * rect_width; 
        sum = sum + area;
    }

    return sum;
}

int main(int argc, char **argv) 
{
  long long i, num_intervals;
  double rect_width, area, sum, x_middle; 

  sscanf(argv[1],"%llu",&num_intervals);

  int myRank;
  int tag = 0;
  int processNum;
  int source;
  int dest = 0;     /* master process's rank */
  long long int workLoadPerProcess;
  int remainLoad;
  long long int localStart;
  long long int localEnd;
  double result;

  MPI_Status status;
  /* Let the system do what it needs to start up MPI */
  MPI_Init(&argc, &argv);
  /* Get my process rank */ 
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
  /* Find out how many processes are being used */ 
  MPI_Comm_size(MPI_COMM_WORLD, &processNum);

  rect_width = PI / num_intervals;
  workLoadPerProcess = num_intervals / processNum;
  remainLoad = num_intervals % processNum;

  if(myRank == 0) {
    localStart = 1;
    localEnd = localStart + workLoadPerProcess + remainLoad - 1;
    result = calculate(localStart, localEnd, rect_width);

    sum = result;
    for(source = 1 ; source < processNum ; source ++) {
        MPI_Recv(&result, 1, MPI_DOUBLE, source, tag, MPI_COMM_WORLD, &status);
        sum += result;
    }
  }
  else {
    localStart = 1 + myRank * workLoadPerProcess + remainLoad;
    localEnd = localStart + workLoadPerProcess - 1;
    result = calculate(localStart, localEnd, rect_width);

    MPI_Send(&result, 1, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD);
  }

  if(myRank == 0)
    printf("The total area is: %f\n", (float)sum);

  /* Shut down MPI */ 
  MPI_Finalize();

  return 0;
}   