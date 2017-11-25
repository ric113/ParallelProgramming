#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>


int isprime(long long int n) {
  long long int i,squareroot;
  if (n>10) {
    squareroot = (long long int) sqrt(n);
    for (i=3; i<=squareroot; i=i+2)
      if ((n%i)==0)
        return 0;
    return 1;
  }
  else
    return 0;
}

int main(int argc, char *argv[])
{
  long long int pc = 4,       /* prime counter , already included {2,3,5,7} */
      foundone; /* most recent prime found */
  long long int n, limit;

  sscanf(argv[1],"%llu",&limit);	
  printf("Starting. Numbers to be scanned= %lld\n",limit);

  int myRank;
  int tag = 0;
  int processNum;
  int source;
  int dest = 0;     /* master process's rank */
  long long int workLoadPerProcess;
  int remainLoad;
  long long int localStart;
  long long int localEnd;
  long long int result[2] = {0,7};  /* index 0 : prime count (included 2,3,5,7), index 1 : largest prime*/
  
  MPI_Status status;
  /* Let the system do what it needs to start up MPI */
  MPI_Init(&argc, &argv);
  /* Get my process rank */ 
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
  /* Find out how many processes are being used */ 
  MPI_Comm_size(MPI_COMM_WORLD, &processNum);

  workLoadPerProcess = ((limit - 11) + 1) / processNum;
  remainLoad = ((limit - 11) + 1) % processNum;

  if(myRank == 0) {
    localStart = 11 + myRank * workLoadPerProcess;
    localEnd = localStart + remainLoad + workLoadPerProcess - 1;

    for(n = localStart ; n <= localEnd ; n = n + 2) {
        if (isprime(n)) {
            result[0] ++ ;
            result[1] = n;
        }	
    }

    // printf("Rank : %d, prime count : %lld, largest prime : %lld\n", myRank, result[0], result[1]);

    pc += result[0];
    foundone = result[1];
    for(source = 1 ; source < processNum ; source ++) {
        MPI_Recv(result, 2, MPI_LONG_LONG, source, tag, MPI_COMM_WORLD, &status);

        pc += result[0];
        if(result[1] > foundone)
            foundone = result[1];
    }
  }
  else {
    localStart = 11 + remainLoad + myRank * workLoadPerProcess;
    localEnd = localStart + workLoadPerProcess - 1;

    /* Avoid local start start at even (will make isprime() error) */
    if(localStart % 2 == 0)
        localStart ++;

    for(n = localStart ; n <= localEnd ; n = n + 2) {
        if (isprime(n)) {
            // printf("Is prime : %lld\n", n);
            result[0] ++ ;
            result[1] = n;
        }	
    }

    // printf("Rank : %d, prime count : %lld, largest prime : %lld\n", myRank, result[0], result[1]);

    MPI_Send(result, 2, MPI_LONG_LONG, dest, tag, MPI_COMM_WORLD);
  }

  if(myRank == 0)
    printf("Done. Largest prime is %lld Total primes %lld\n",foundone,pc);

  /* Shut down MPI */ 
  MPI_Finalize();

  return 0;
} 