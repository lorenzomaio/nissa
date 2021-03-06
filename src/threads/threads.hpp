#ifndef _THREADS_HPP
#define _THREADS_HPP

#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

#define NISSA_CHUNK_WORKLOAD(START,CHUNK_LOAD,END,EXT_START,EXT_END,CHUNK_ID,NCHUNKS) \
  int WORKLOAD=EXT_END-EXT_START,					\
  CHUNK_LOAD=(WORKLOAD+NCHUNKS-1)/NCHUNKS,				\
  START=EXT_START+CHUNK_ID*CHUNK_LOAD,					\
  END=START+CHUNK_LOAD< EXT_END ? START+CHUNK_LOAD : EXT_END

#define NISSA_CHUNK_LOOP(INDEX,EXT_START,EXT_END,CHUNK_ID,NCHUNKS)	\
  for(NISSA_CHUNK_WORKLOAD(START,CHUNK_LOAD,END,EXT_START,EXT_END,CHUNK_ID,NCHUNKS),INDEX=START;INDEX<END;INDEX++)

#define THREADABLE_FUNCTION_END }

#if THREADS_TYPE == NO_THREADS
 #include "threads/no_threads.hpp"
#elif THREADS_TYPE == OPENMP_THREADS
 #include "threads/openmp_threads.hpp"
#elif THREADS_TYPE == CUDA_THREADS
 #include "threads/cuda_threads.hpp"
#else
 #error Unknown thread paralleization THREADS_TYPE !
#endif

namespace nissa
{
}

#endif
