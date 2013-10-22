#ifndef _rootst_eoimpr_rhmc_step_hpp
#define _rootst_eoimpr_rhmc_step_hpp

namespace nissa
{
  double rootst_eoimpr_rhmc_step(quad_su3 **out_conf,quad_su3 **in_conf,theory_pars_t &theory_pars,
				 hmc_evol_pars_t &simul_pars,int itraj);
}

#endif