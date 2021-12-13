#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <omp.h>
#include <chrono>

#ifdef __cplusplus
extern "C" {
#endif
  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (int* arr, size_t n);
#ifdef __cplusplus
}
#endif

void merge(int* arr, int l, int mid, int r) {
  if (l == r) return;

  if (r-l == 1) {
    if (arr[l] > arr[r]) {
      int temp = arr[l];
      arr[l] = arr[r];
      arr[r] = temp;
    }
    return;
  }

  int i, j, k;
  int n = mid - l;
  
  int *temp = new int[n];

  for (i=0; i<n; ++i)
    temp[i] = arr[l+i];

  i = 0;    
  j = mid;  
  k = l;    

  while (i<n && j<=r) {
    if (temp[i] <= arr[j] ) {
      arr[k++] = temp[i++];
    } else {
      arr[k++] = arr[j++];
    }
  }

  while (i<n) {
    arr[k++] = temp[i++];
  }

  delete[] temp;

}


void mergeSort(int * arr, int l, int r) {

  if (l < r) {
    int mid = (l+r)/2;

    #pragma omp parallel 
    {
        #pragma omp single
      {
            #pragma omp task
	mergeSort(arr, l, mid);

            #pragma omp task
	mergeSort(arr, mid+1, r);

            #pragma omp taskwait
	merge(arr, l, mid+1, r);
      }
    } 
    
  }

}

int main (int argc, char* argv[]) {
  
  if (argc < 3) { std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  int nbthread = atoi(argv[2]);

  omp_set_num_threads(nbthread);
  
  // get arr data
  int* arr = new int [n];

  int* tempArr = new int[n];

  generateMergeSortData (arr, n);

  //insert sorting code here.
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

  #pragma omp parallel
  {
    #pragma omp single
    {
      mergeSort(arr, 0, n-1);
    }
  }

  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;
  
  checkMergeSortResult (arr, n);

  std::cerr<<elapsed_seconds.count()<<std::endl;
  
  delete[] arr;

  return 0;
}
