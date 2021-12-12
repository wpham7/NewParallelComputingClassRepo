#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <cmath>

#include <vector>
#include "seq_loop.hpp"
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

float integral (float functionid, float a, float b, int n, int intensity, int nbthreads)
{
  SequentialLoop s(n, nbthreads);
  float f=0;
  float difference = (b-a);
  float c = difference/n;
  std::vector <float> fvector;

  s.parfor<float(0,n,1,[&]float& function) -> void
  {
    function = 0;
  }, [&] (int i, float& function) -> void
  {
    float integral = (a+((i+0.5)*c));
  if(functionid==1)
    function = function + f1(integral,intensity);
  else if(functionid==2)
    function = function + f2(integral,intensity);
  else if(functionid==3)
    function = function + f3(integral, intensity);
  else if(functionid==4)
    function = function + f4(integral,intensity);
  else
    std::cout<<"Error occured!\n";
  }, [&] (float &function) -> void
  {
    f = f + function;
  }
  );

  return (f * c);
}

int main (int argc, char* argv[]) {

  if (argc < 7) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads>"<<std::endl;
    return -1;
  }
  float function = atoi(argv[1]);
  float a = strtof(argv[2],NULL);
  float b = strtof(argv[3],NULL);
  int n = atoi(argv[4]);
  int intensity = atoi(argv[5]);
  int nbthreads = atoi(argv[6]);

  auto starttime = std::chrono::system_clock::now();

  float function = (float) integral(functionid,a,b,n,intensity,nbthreads);

  auto endtime = std::chrono::system_clock::now();

  std::chrono::duration<double> timetaken = endtime - starttime;

  std::cout<<function<<"\n";
  std::cerr<<timetaken.count()<<"\n";
  return 0;
}
