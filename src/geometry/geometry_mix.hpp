#ifndef _GEOMETRY_MIX_HPP
#define _GEOMETRY_MIX_HPP

#include <unistd.h>

namespace nissa
{
  void paste_eo_parts_into_lx_vector_internal(char *out_lx,char **in_eo,size_t bps);
  template <class T> void paste_eo_parts_into_lx_vector(T *out_lx,T **in_eo)
  {paste_eo_parts_into_lx_vector_internal((char*)out_lx,(char**)in_eo,sizeof(T));}
  
  
  void split_lx_vector_into_eo_parts_internal(char **out_eo,char *in_lx,size_t bps);
  template <class T> void split_lx_vector_into_eo_parts(T **out_eo,T *in_lx)
  {split_lx_vector_into_eo_parts_internal((char**)out_eo,(char*)in_lx,sizeof(T));}

}

#endif
