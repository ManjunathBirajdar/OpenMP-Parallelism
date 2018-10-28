#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>
#include <cmath>

#ifdef __cplusplus
extern "C" {
#endif
  void generatePrefixSumData (int* arr, size_t n);
  void checkPrefixSumResult (int* arr, size_t n);
#ifdef __cplusplus
}
#endif


int main (int argc, char* argv[]) {
  //forces openmp to create the threads beforehand
	#pragma omp parallel
  {
    int fd = open (argv[0], O_RDONLY);
    if (fd != -1) {
      close (fd);
    }
    else {
      std::cerr<<"something is amiss"<<std::endl;
    }
  }
  
  if (argc < 3) {
    std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  
  int n = atoi(argv[1]);
  int nbthreads = atoi(argv[2]);		// numbr of threads
  int * arr = new int [n];
  generatePrefixSumData (arr, n);

  int * pr = new int [n+1];
  
	int slot=ceil((float)n/(float)nbthreads);
	omp_set_num_threads(nbthreads);
	
	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  #pragma omp parallel for schedule(guided,1000)
	for (int i=0; i<nbthreads; ++i) {		
		pr[i*slot]=arr[i*slot];
		for(int j=(i*slot)+1;j<(i+1)*slot && j<n;++j) {
			pr[j] = pr[j-1] + arr[j];
		}		
	}	

	int diff;
	int k=slot;
	while(k<n){
		if(k%slot==0){
			diff=pr[k-1];
			for(int j=k;j<(k+slot) && j<n;++j) {
				pr[j]=pr[j]+diff;
			}
			k=k+slot;
		}	else{
			k=k+1;
		} 
	}
	for( int index =n-1; index >= 0 ; index-- ) {
		pr[index+1] = pr [index];
	}
	pr[0]=arr[0];
	
	std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end-start;
	std::cerr<<elapsed_seconds.count()<<std::endl;
  
	checkPrefixSumResult(pr, n);

	delete[] arr;

  return 0;
}
