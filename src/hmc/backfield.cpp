#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

#include <math.h>

#include "base/global_variables.hpp"
#include "base/thread_macros.hpp"
#include "base/vectors.hpp"
#include "new_types/complex.hpp"
#include "new_types/new_types_definitions.hpp"
#include "new_types/su3.hpp"
#include "routines/ios.hpp"
#ifdef USE_THREADS
 #include "routines/thread.hpp"
#endif

namespace nissa
{
  //initialize an u(1) field to unity
  void init_backfield_to_id(quad_u1 **S)
  {
    for(int par=0;par<2;par++)
      {
	NISSA_LOC_VOLH_LOOP(ivol)
	  for(int mu=0;mu<4;mu++)
	    {
	      S[par][ivol][mu][0]=1;
	      S[par][ivol][mu][1]=0;
	    }
        set_borders_invalid(S[par]);
      }
  }
  
  //multiply a background field by the imaginary chemical potential
  void add_im_pot_to_backfield(quad_u1 **S,quark_content_t &quark_content)
  {
    double im_pot=quark_content.im_pot*M_PI/glb_size[0];
    complex ph={cos(im_pot),sin(im_pot)};
    
    for(int par=0;par<2;par++)
      {
	NISSA_LOC_VOLH_LOOP(ieo)
	  safe_complex_prod(S[par][ieo][0],S[par][ieo][0],ph);
	set_borders_invalid(S[par]);
      }
  }
  
  //compute args for 1/L2 quantization
  void get_args_of_one_over_L2_quantization(coords phase,int ivol,int mu,int nu)
  {
    //reset
    phase[0]=phase[1]=phase[2]=phase[3]=0;
    
    //take absolute coords
    int xmu=glb_coord_of_loclx[ivol][mu];
    int xnu=glb_coord_of_loclx[ivol][nu];
    if(xmu>=glb_size[mu]/2) xmu-=glb_size[mu];
    if(xnu>=glb_size[nu]/2) xnu-=glb_size[nu];
    
    //define the arguments of exponentials
    if(xmu==glb_size[mu]/2-1) phase[mu]=-xnu*glb_size[mu];
    phase[nu]=xmu;
  }
  
  //multiply a background field by a constant em field
  //mu nu refers to the entry of F_mu_nu involved
  void add_em_field_to_backfield(quad_u1 **S,quark_content_t &quark_content,double em_str,int mu,int nu)
  {
    double phase=2*em_str*quark_content.charge*M_PI/glb_size[mu]/glb_size[nu];
    
    for(int par=0;par<2;par++)
      {
	NISSA_LOC_VOLH_LOOP(ieo)
        {
	  //compute arg
	  coords arg;
	  get_args_of_one_over_L2_quantization(arg,loclx_of_loceo[par][ieo],mu,nu);
	  
	  //compute u1phase and multiply
	  for(int rho=0;rho<4;rho++)
	    {
	      complex u1phase={cos(phase*arg[rho]),sin(phase*arg[rho])};
	      safe_complex_prod(S[par][ieo][rho],S[par][ieo][rho],u1phase);
	    }
	}
	
	set_borders_invalid(S[par]);
      }
  }
  
  //set up all the 6 components
  void add_em_field_to_backfield(quad_u1 **S,quark_content_t &quark_content,em_field_pars_t &em_field_pars)
  {
    add_em_field_to_backfield(S,quark_content,em_field_pars.E[0],0,1);
    add_em_field_to_backfield(S,quark_content,em_field_pars.E[1],0,2);
    add_em_field_to_backfield(S,quark_content,em_field_pars.E[2],0,3);
    add_em_field_to_backfield(S,quark_content,em_field_pars.B[0],2,3);
    add_em_field_to_backfield(S,quark_content,em_field_pars.B[1],3,1);
    add_em_field_to_backfield(S,quark_content,em_field_pars.B[2],1,2);
  }
  
  //multiply the configuration for an additional u(1) field
  THREADABLE_FUNCTION_2ARG(add_backfield_to_conf, quad_su3**,conf, quad_u1**,u1)
  {
    verbosity_lv2_master_printf("Adding backfield\n");
    GET_THREAD_ID();
    for(int par=0;par<2;par++)
      {
	NISSA_PARALLEL_LOOP(ivol,0,loc_volh)
	  for(int mu=0;mu<4;mu++)
	    safe_su3_prod_complex(conf[par][ivol][mu],conf[par][ivol][mu],u1[par][ivol][mu]);
	set_borders_invalid(conf[par]);
      }
  }}

  //multiply the configuration for an the conjugate of an u(1) field
  THREADABLE_FUNCTION_2ARG(rem_backfield_from_conf, quad_su3**,conf, quad_u1**,u1)
  {
    verbosity_lv2_master_printf("Removing backfield\n");
    GET_THREAD_ID();
    for(int par=0;par<2;par++)
      {
	NISSA_PARALLEL_LOOP(ivol,0,loc_volh)
	  for(int mu=0;mu<4;mu++)
	    safe_su3_prod_conj_complex(conf[par][ivol][mu],conf[par][ivol][mu],u1[par][ivol][mu]);
	set_borders_invalid(conf[par]);
      }
  }}
}
