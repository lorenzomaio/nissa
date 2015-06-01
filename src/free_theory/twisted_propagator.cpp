#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

#include "free_theory_types.hpp"
#include "free_theory_types_routines.hpp"
#include "cg_eoprec_twisted_free_operator.hpp"

#include "base/global_variables.hpp"
#include "base/thread_macros.hpp"
#include "geometry/geometry_lx.hpp"
#include "new_types/complex.hpp"
#include "new_types/dirac.hpp"
#include "new_types/spin.hpp"
#include "operations/fourier_transform.hpp"

#ifdef USE_THREADS
 #include "routines/thread.hpp"
#endif

#include "twisted_propagator.hpp"

namespace nissa
{
  //compute the energy of a twisted mass quark
  double tm_quark_energy(tm_quark_info qu,int imom)
  {
    double m0=m0_of_kappa(qu.kappa);
    double mass=qu.mass;
    double m2=m0*m0+mass*mass;
    double p2=0,p4=0;
    coords c;
    glb_coord_of_glblx(c,imom);
    for(int mu=1;mu<NDIM;mu++)
      {
	double p=M_PI*(2*c[mu]+qu.bc[mu])/glb_size[mu];
	double sinph=sin(p/2);
	double sinph2=sinph*sinph;
	double sinph4=sinph2*sinph2;
	p2+=sinph2;
	p4+=sinph4;
      }
    p2*=4;
    p4*=4;
    
    double four_sinh2_Eh=(m2+p2*(1+m0)+p2*p2/4-p4)/(1+m0+p2/2);
    
    return 2*asinh(sqrt(four_sinh2_Eh/4));
  }

  //compute the energy of a naive massless fermion
  double naive_massless_quark_energy(momentum_t bc,int imom)
  {
    double sinh2E=0;
    coords c;
    glb_coord_of_glblx(c,imom);
    for(int mu=1;mu<NDIM;mu++) sinh2E+=sqr(sin(M_PI*(2*c[mu]+bc[mu])/glb_size[mu]));
    return asinh(sqrt(sinh2E));
  }

  ////////////////////////////////////////////// twisted propagator in momentum space ////////////////////////////////////////////

  //return sin(p), \sum sin(p)^2, \sum sin(p/2)^2
  void get_component_of_twisted_propagator_of_imom(momentum_t sin_mom,double &sin2_mom,double &sin2_momh,tm_quark_info qu,int imom)
  {
    sin2_mom=sin2_momh=0;
    for(int mu=0;mu<NDIM;mu++)
      {
	double p=M_PI*(2*glb_coord_of_loclx[imom][mu]+qu.bc[mu])/glb_size[mu];
	sin_mom[mu]=sin(p);
	sin2_mom+=sqr(sin_mom[mu]);
	sin2_momh+=sqr(sin(p/2));
      }
  }
  
  //takes the dirac operator of fixed momentum
  void mom_space_twisted_operator_of_imom(spinspin out,tm_quark_info qu,int imom)
  {
    //takes the momenta part and M
    momentum_t sin_mom;
    double sin2_mom,sin2_momh;
    get_component_of_twisted_propagator_of_imom(sin_mom,sin2_mom,sin2_momh,qu,imom);
    double M=m0_of_kappa(qu.kappa)+2*sin2_momh;
    
    //fill the pieces
    spinspin_put_to_zero(out);  
    spinspin_dirac_summ_the_prod_double(out,&base_gamma[0],qu.mass);
    for(int mu=0;mu<4;mu++) spinspin_dirac_summ_the_prod_idouble(out,base_gamma+map_mu[mu],sin_mom[mu]);
    spinspin_dirac_summ_the_prod_idouble(out,&base_gamma[5],-M*tau3[qu.r]);
  }
  
  //single momentum - normalisation is such that D*S=1/vol
  void mom_space_twisted_propagator_of_imom(spinspin prop,tm_quark_info qu,int imom)
  {
    //takes the momenta part
    momentum_t sin_mom;
    double sin2_mom,sin2_momh;
    get_component_of_twisted_propagator_of_imom(sin_mom,sin2_mom,sin2_momh,qu,imom);

    //compute M and the denominator
    double M=m0_of_kappa(qu.kappa)+2*sin2_momh;
    double den=sin2_mom+sqr(M)+sqr(qu.mass);

    //fill the pieces
    spinspin_put_to_zero(prop);  
    if(fabs(den)>=1.e-14)
      {
	//for efficiency
	double rep_den=1/den/glb_vol;
	
	spinspin_dirac_summ_the_prod_double(prop,&base_gamma[0],qu.mass*rep_den);
	for(int mu=0;mu<NDIM;mu++) spinspin_dirac_summ_the_prod_idouble(prop,base_gamma+map_mu[mu],-sin_mom[mu]*rep_den);
	spinspin_dirac_summ_the_prod_idouble(prop,&base_gamma[5],M*tau3[qu.r]*rep_den);
      }
    else
      for(int ig=0;ig<4;ig++)
	complex_prod_double(prop[ig][base_gamma[0].pos[ig]],base_gamma[0].entr[ig],qu.zmp);
  }

  //replace p0 with on shell in the tilded (conjugated) or non-tilded dirac operator
  //the sign of the energy is according to passed argument
  double twisted_on_shell_operator_of_imom(spinspin proj,tm_quark_info qu,int imom,bool tilded,int esign)
  {
    if(esign!=-1&&esign!=+1) crash("illegal energy sign\"%d\"",esign);
    double abse=tm_quark_energy(qu,imom);
    double e=esign*abse;
    
    momentum_t sin_mom;
    double sin2_mom=-sqr(sinh(e));
    double sin2_momh=-sqr(sinh(e/2));
    coords c;
    glb_coord_of_glblx(c,imom);
    for(int mu=1;mu<4;mu++)
      {
	double p=M_PI*(2*c[mu]+qu.bc[mu])/glb_size[mu];
	sin_mom[mu]=sin(p);
	sin2_mom+=sqr(sin_mom[mu]);
	sin2_momh+=sqr(sin(p/2));
      }
    double M=m0_of_kappa(qu.kappa)+2*sin2_momh;
    
    spinspin_put_to_diag(proj,qu.mass);
    int se[2]={-1,+1},sp[2]={+1,-1},s5[2]={-1,+1}; //we put here implicitly the difference of g5 with Nazario
    spinspin_dirac_summ_the_prod_double(proj,base_gamma+map_mu[0],se[tilded]*sinh(e));
    for(int mu=1;mu<4;mu++) spinspin_dirac_summ_the_prod_idouble(proj,base_gamma+map_mu[mu],sp[tilded]*sin_mom[mu]);
    spinspin_dirac_summ_the_prod_idouble(proj,base_gamma+5,s5[tilded]*M*tau3[qu.r]);
    
    return abse;
  }

  //same for the naive fermions
  double naive_massless_on_shell_operator_of_imom(spinspin proj,momentum_t bc,int imom,int esign)
  {
    if(esign!=-1&&esign!=+1) crash("illegal energy sign\"%d\"",esign);
    double abse=naive_massless_quark_energy(bc,imom);
    double e=esign*abse;
    
    spinspin_dirac_prod_double(proj,base_gamma+map_mu[0],-sinh(e));
    coords c;
    glb_coord_of_glblx(c,imom);
    for(int mu=1;mu<4;mu++) spinspin_dirac_summ_the_prod_idouble(proj,base_gamma+map_mu[mu],sin(M_PI*(2*c[mu]+bc[mu])/glb_size[mu]));
    
    return abse;
  }
  
  //eigenvectors of (1-+g0)/2
  double W=1/sqrt(2);
  //aparticle are associated with         phi_s, eigenvectors of (1-g0)
  //particles are associated with \tilde{phi}_s, eigenvectors of (1+g0)
  //so take ompg0_eig[!par_apar][s]
  spin ompg0_eig[2][2]={{{{+W, 0},{ 0, 0},{+W, 0},{ 0, 0}},
			 {{ 0, 0},{+W, 0},{ 0, 0},{+W, 0}}},
			{{{+W, 0},{ 0, 0},{-W, 0},{ 0, 0}},
			 {{ 0, 0},{+W, 0},{ 0, 0},{-W, 0}}}};
  
  //return the wave function of "u_r(p)" (particle) or "v_r(-p)" (antiparticle)
  void twisted_wavefunction_of_imom(spin wf,tm_quark_info qu,int imom,int par_apar,int s)
  {
    //particle:  u_r(+p)=\tilde{D}(iE,p) \phi^tilde_r/sqrt(m+sinh(e))
    //aparticle: v_r(-p)=G0 D(iE,p) \phi_r/sqrt(m+sinh(e))
    bool tilde[2]={true,false};
    spinspin osp;
    double e=twisted_on_shell_operator_of_imom(osp,qu,imom,tilde[par_apar],1);
    unsafe_spinspin_prod_spin(wf,osp,ompg0_eig[!par_apar][s]);
    spin_prodassign_double(wf,1/sqrt(qu.mass+sinh(e)));
    int ig[2]={0,map_mu[0]};
    safe_dirac_prod_spin(wf,base_gamma+ig[par_apar],wf);
  }

  //same for naive massless fermions
  void naive_massless_wavefunction_of_imom(spin wf,momentum_t bc,int imom,int par_apar,int s)
  {
    //particle:  u_r(+p)=-D(iE,p) \phi^tilde_r/sqrt(sinh(e))
    //aparticle: v_r(-p)=-D(-iE,p) \phi_r/sqrt(sinh(e))
    spinspin osp;
    int esign[2]={+1,-1};
    double e=naive_massless_on_shell_operator_of_imom(osp,bc,imom,esign[par_apar]);
    unsafe_spinspin_prod_spin(wf,osp,ompg0_eig[!par_apar][s]);
    spin_prodassign_double(wf,-1/sqrt(sinh(e)));
  }
  
  //whole quark propagator in momentum space
  THREADABLE_FUNCTION_2ARG(compute_mom_space_twisted_propagator, spinspin*,prop, tm_quark_info,qu)
  {
    GET_THREAD_ID();
    
    NISSA_PARALLEL_LOOP(imom,0,loc_vol)
      mom_space_twisted_propagator_of_imom(prop[imom],qu,imom);
    
    set_borders_invalid(prop);
  }
  THREADABLE_FUNCTION_END
  
  ///////////////////////////////////////////// twisted propagator in x space ////////////////////////////////////////////////

  //single
  void compute_x_space_twisted_propagator_by_fft(spinspin *prop,tm_quark_info qu)
  {
    compute_mom_space_twisted_propagator(prop,qu);
    pass_spinspin_from_mom_to_x_space(prop,prop,qu.bc);
  }
  
  //squared (scalar insertion)
  THREADABLE_FUNCTION_2ARG(compute_x_space_twisted_squared_propagator_by_fft, spinspin*,sq_prop, tm_quark_info,qu)
  {
    GET_THREAD_ID();
    
    compute_mom_space_twisted_propagator(sq_prop,qu);
    
    //square (including normalisation)
    NISSA_PARALLEL_LOOP(imom,0,loc_vol)
      {
	safe_spinspin_prod_spinspin(sq_prop[imom],sq_prop[imom],sq_prop[imom]);
	spinspin_prodassign_double(sq_prop[imom],glb_vol);
      }
    THREAD_BARRIER();
    
    pass_spinspin_from_mom_to_x_space(sq_prop,sq_prop,qu.bc);
  }
  THREADABLE_FUNCTION_END
  
  /////////////////////////////////////////////// multiply from left or right a spin ///////////////////////////////////////////////

  //multiply from left
#define DEFINE_MULTIPLY_FROM_LEFT_OR_RIGHT_BY_MOM_SPACE_TWISTED_PROPAGATOR(TYPE) \
  THREADABLE_FUNCTION_3ARG(multiply_from_left_by_mom_space_twisted_propagator, TYPE*,out, TYPE*,in, tm_quark_info,qu) \
  {									\
    GET_THREAD_ID();							\
    									\
    NISSA_PARALLEL_LOOP(imom,0,loc_vol)					\
      {									\
	spinspin prop;							\
	mom_space_twisted_propagator_of_imom(prop,qu,imom);		\
	NAME2(safe_spinspin_prod,TYPE)(out[imom],prop,in[imom]);	\
      }									\
    									\
    set_borders_invalid(out);						\
  }									\
  THREADABLE_FUNCTION_END						\
									\
  /*multiply from right*/						\
  THREADABLE_FUNCTION_3ARG(multiply_from_right_by_mom_space_twisted_propagator, TYPE*,out, TYPE*,in, tm_quark_info,qu) \
  {									\
    GET_THREAD_ID();							\
									\
    NISSA_PARALLEL_LOOP(imom,0,loc_vol)					\
      {									\
	spinspin prop;							\
	mom_space_twisted_propagator_of_imom(prop,qu,imom);		\
	NAME3(safe,TYPE,prod_spinspin)(out[imom],in[imom],prop);	\
      }									\
    									\
    set_borders_invalid(out);						\
  }									\
  THREADABLE_FUNCTION_END						\
  
  DEFINE_MULTIPLY_FROM_LEFT_OR_RIGHT_BY_MOM_SPACE_TWISTED_PROPAGATOR(spin);
  DEFINE_MULTIPLY_FROM_LEFT_OR_RIGHT_BY_MOM_SPACE_TWISTED_PROPAGATOR(spinspin);
  
  ////////////////////////////////////////////// by inversion /////////////////////////////////////////////
  
  //multiply the source for the twisted propagator by inverting twisted Dirac operator
  void multiply_from_left_by_x_space_twisted_propagator_by_inv(spin *prop,spin *ext_source,tm_quark_info qu)
  {
    crash("not yet in phys basis");
    inv_tmD_cg_eoprec_eos(prop,NULL,qu,1000000,1.e-28,ext_source);
  }
  void multiply_from_left_by_x_space_twisted_propagator_by_inv(spinspin *prop,spinspin *ext_source,tm_quark_info qu)
  {
    //source and temp prop
    spin *tsource=nissa_malloc("tsource",loc_vol+bord_vol,spin);
    spin *tprop=nissa_malloc("tprop",loc_vol,spin);
    
    //loop over the source index
    for(int id_so=0;id_so<4;id_so++)
      {
	get_spin_from_spinspin(tsource,ext_source,id_so);
	multiply_from_left_by_x_space_twisted_propagator_by_inv(tprop,tsource,qu);
	put_spin_into_spinspin(prop,tprop,id_so);
      }
    
    set_borders_invalid(prop);
    
    nissa_free(tsource);
    nissa_free(tprop);
  }
  
  //prepare it by inverting
  void compute_x_space_twisted_propagator_by_inv(spinspin *prop,tm_quark_info qu)
  {
    //allocate a source
    spinspin *delta=nissa_malloc("delta",loc_vol+bord_vol,spinspin);
    vector_reset(delta);
    if(rank==0) spinspin_put_to_id(delta[0]);
    set_borders_invalid(delta);
    
    multiply_from_left_by_x_space_twisted_propagator_by_inv(prop,delta,qu);
    
    set_borders_invalid(prop);
    
    nissa_free(delta);
  }
}