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

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif
using namespace std;


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
  
  if (argc < 9) {
    std::cerr<<"Usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <scheduling> <granularity>"<<std::endl;
    return -1;
  }

  int functionid, intensity, nbthreads;
  int n;
  float a,b,granularity;
  functionid = atoi(argv[1]);				// an integer to know which function to integrate
  a = atof(argv[2]);								// the lower bound of the integral
  b = atof(argv[3]);								// the upper bound of the integral
  n = atoi(argv[4]);								// an integer which is the no. of points to compute the approximation of the integral
  intensity = atoi(argv[5]);				// an integer which is the second parameter to give the the function to integrate
  nbthreads = atoi(argv[6]);				// number of threads
  std::string schedPolicy = argv[7];// a string either iteration or thread or chunk that indicate which sync method to use
  granularity = atoi(argv[8]);			// an integer indicating how many iterations does one thread take at a time

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
	
	float result = 0,input;
  float y = (b-a)/n;								// Calculate resultue of (b-a)/n and store it in b_a_by_n
	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	
	switch(functionid) {
		case 1 :
	 		#pragma omp parallel for reduction(+:result) schedule(runtime)
			for(int i=0;i<n;i++) {
				input = a + ((i+0.5)*y);
				result += f1(input,intensity);
			}
			break;
		case 2 :
			#pragma omp parallel for reduction(+:result) schedule(runtime)
			for(int i=0;i<n;i++) {
				input = a + ((i+0.5)*y);
				result += f2(input,intensity);
			}
			break;
		case 3:
			#pragma omp parallel for reduction(+:result) schedule(runtime)
			for(int i=0;i<n;i++) {
				input = a + ((i+0.5)*y);
				result += f3(input,intensity);
			}
			break;
		case 4: 
			#pragma omp parallel for reduction(+:result) schedule(runtime)
			for(int i=0;i<n;i++) {
				input = a + ((i+0.5)*y);
				result += f4(input,intensity);
			}
			break;
		default : break;
  	}
	
  result = y*result;					// Calculate resultue of F(x)*(b-a)/n where F(x) is the integration of f(x)
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;
  std::cout<<result<<std::endl;
  std::cerr<<elapsed_seconds.count()<<std::endl;

  return 0;
}
