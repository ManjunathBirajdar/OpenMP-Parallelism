#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>


#ifdef __cplusplus
extern "C" {
#endif
  void generateReduceData (int* arr, size_t n);
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
  
  if (argc < 5) {
    std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads> <scheduling> <granularity>"<<std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  int nbthreads = atoi(argv[2]);
  std::string schedPolicy = argv[3];
  int granularity = atoi(argv[4]);
  int * arr = new int [n];
	omp_set_num_threads(nbthreads);
	if(schedPolicy.compare("static")==0){
		omp_set_schedule(omp_sched_static,granularity);
	}
	else if(schedPolicy.compare("dynamic")==0){
		omp_set_schedule(omp_sched_dynamic,granularity);
	}
	else if(schedPolicy.compare("guided")==0){
		omp_set_schedule(omp_sched_guided,granularity);
	}
	else {
		return -1;
	}
	
	
  generateReduceData (arr, atoi(argv[1]));
	int result = 0;
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  #pragma omp parallel for reduction(+:result) schedule(runtime)
  for ( int i=0; i < n ; ++ i ){
		result += arr [i];
  }
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;
  std::cout<<result<<std::endl;
  std::cerr<<elapsed_seconds.count()<<std::endl;
  delete[] arr;

  return 0;
}
