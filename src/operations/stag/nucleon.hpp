#ifndef _NUCLEON_HPP
#define _NUCLEON_HPP

#include "hmc/theory_pars.hpp"

namespace nissa
{
  //parameters to compute time nucleon correlator
  struct nucleon_corr_meas_pars_t
  {
    int each;
    int after;
    std::string path;
    double residue;
    int nhits;
    
    int def_each(){return 1;}
    int def_after(){return 0;}
    std::string def_path(){return "nucleon_corr";}
    double def_residue(){return 1e-12;}
    int def_nhits(){return 1;}
    
    int master_fprintf(FILE *fout,bool full);
    
    int is_nonstandard()
    {
      return
	each!=def_each()||
	after!=def_after()||
	path!=def_path()||
	residue!=def_residue()||
	nhits!=def_nhits();
    }
    
    nucleon_corr_meas_pars_t() :
      each(def_each()),
      after(def_after()),
      path(def_path()),
      residue(def_residue()),
      nhits(def_nhits()) {}
  };
  
  void measure_nucleon_corr(quad_su3 **conf,theory_pars_t theory_pars,nucleon_corr_meas_pars_t meas_pars,int iconf,int conf_created);
}

#endif
