#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include <math.h>
#include <stdlib.h>
#include <chrono>
#include <string.h>
#ifdef __cplusplus
extern "C" {
#endif

  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (int* arr, size_t n);

  
#ifdef __cplusplus
}
#endif

void merge(int arr[],int left,int mid,int right,int arr1[])
{
    int i=left, j=mid;
    for (int k=left; k<right; k++) {
        if (i<mid && (j>=right || arr[i]<=arr[j])) {
            arr1[k] = arr[i];
            i = i+1;
        } else {
            arr1[k] = arr[j];
            j = j+1;    
        }
    } 
}

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

  int * arr = new int [atoi(argv[1])];
  int * arr1 = new int [atoi(argv[1])];


  generateMergeSortData (arr, atoi(argv[1]));
  
  int n = atoi(argv[1]);
  int nbthreads = atoi(argv[2]);
   
  omp_set_num_threads(nbthreads);
 
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  
  for (int size=1; size<n; size=2*size) {
    #pragma omp parallel for schedule(static)
    for (int i=0; i<n; i=i+2*size) {
      merge(arr, i, std::min(i+size, n), std::min(i+2*size, n), arr1);
    }
    
    #pragma omp parallel for schedule(static)
    for(int i = 0; i < n; i++) {		//Copy the contents of arr1 to arr
        arr[i] = arr1[i];
    }
  }

  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapsed_seconds = end-start;
  
  std::cerr<<elapsed_seconds.count()<<std::endl;
  
  checkMergeSortResult (arr, atoi(argv[1]));  
 
  delete[] arr;
  

return 0;
}

