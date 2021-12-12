#ifndef __SEQ_LOOP_H
#define __SEQ_LOOP_H

#include <functional>
#include <iostream>
#include <vector>
#include <thread>

class SequentialLoop {
public:
  /// @brief execute the function f multiple times with different
  /// parameters possibly in parallel
  ///
  /// f will be executed multiple times with parameters starting at
  /// beg, no greater than end, in inc increment. These execution may
  /// be in parallel
  int n, nbthreads;
  SequentialLoop(int n, int nbthreads)
  {
    this -> n = n;
    this -> nbthreads = nbthreads;
  }

  void parfor (size_t beg, size_t end, size_t inc,
	       std::function<void(int)> f) {
    for (size_t i=beg; i<end; i+= inc) {
      f(i);
    }
  }

  /// @brief execute the function f multiple times with different
  /// parameters possibly in parallel
  ///
  /// f will be executed multiple times with parameters starting at
  /// beg, no greater than end, in inc increment. These execution may
  /// be in parallel.
  ///
  /// Each thread that participate in the calculation will have its
  /// own TLS object.
  ///
  /// Each thread will execute function before prior to any
  /// calculation on a TLS object.
  ///
  /// Each thread will be passed the TLS object it executed before on.
  ///
  /// Once the iterations are complete, each thread will execute after
  /// on the TLS object. No two thread can execute after at the same time.
  template<typename TLS>
  void parfor (size_t beg, size_t end, size_t increment,
	       std::function<void(TLS&)> before,
	       std::function<void(int, TLS&)> f,
	       std::function<void(TLS&)> after
	       ) {
    std::vector<TLS> functionvector;
    std::vector<std::thread> mythreads;

    for(int i = 0; i < nbthreads; i++)
      {
        TLS tls;
        functionvector.push_back(tls);
      }

    int a = (n/nbthreads);

    for(int t = 0; t < nbthreads; t++)
    {
      before(functionvector[t]);
      mythreads.push_back(std::thread([&](size_t beg, size_t end, int increment, TLS &tls){
        for (size_t i = beg; i < end; i += increment)
        {
          f(i,tls);
        }
      },
      t * a, (nbthreads - 1) == t ? n: (t * a) + a, 1, std::ref(functionvector[t])));
      }

      for (auto& thread : mythreads)
      {
        thread.join();
      }

      for (auto& b : functionvector)
      {
        after(b);
      }
  }
  
};

#endif
