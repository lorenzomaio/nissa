#ifndef _OPENMP_H
#define _OPENMP_H

#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

#include "base/thread_macros.hpp"

namespace nissa
{
#ifdef THREAD_DEBUG
  void thread_barrier(const char*file,int line,int force_barrier);
#else
  void thread_barrier(int force_barrier);
#endif
  
  void start_threaded_function(void(*function)(void),const char *name);
  void thread_master_start(int narg,char **arg,void(*main_function)(int narg,char **arg));
  void thread_pool();
  void thread_pool_stop();
}

#endif
