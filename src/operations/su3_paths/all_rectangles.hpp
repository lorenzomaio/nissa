#ifndef _ALL_RECTANGLES_HPP
#define _ALL_RECTANGLES_HPP

#include "operations/smearing/smooth.hpp"
#include "routines/ios.hpp"

namespace nissa
{
  //parameters to measure all rectangles path
  struct all_rect_meas_pars_t
  {
    int each;
    int after;
    std::string path;
    smooth_pars_t smear_pars;
    int Tmin,Tmax,Dmin,Dmax;
    
    int def_each(){return 1;}
    int def_after(){return 0;}
    std::string def_path(){return "rectangles";}
    int def_Tmin(){return 3;}
    int def_Tmax(){return 9;}
    int def_Dmin(){return 1;}
    int def_Dmax(){return 9;}
    
    int is_nonstandard()
    {
      return
	each!=def_each()||
	after!=def_after()||
	path!=def_path()||
	Tmin!=def_Tmin()||
	Tmax!=def_Tmax()||
	Dmin!=def_Dmin()||
	Dmax!=def_Dmax()||
	smear_pars.is_nonstandard();
    }
    
    all_rect_meas_pars_t() :
      each(def_each()),
      after(def_after()),
      path(def_path()),
      Tmin(def_Tmin()),
      Tmax(def_Tmax()),
      Dmin(def_Dmin()),
      Dmax(def_Dmax()) {}
  };
  
  void measure_all_rectangular_paths(all_rect_meas_pars_t *pars,quad_su3  *conf,int iconf,int create_output_file);
  void measure_all_rectangular_paths_old(all_rect_meas_pars_t *pars,quad_su3  *conf,int iconf,int create_output_file);
  void measure_all_rectangular_paths(all_rect_meas_pars_t *pars,quad_su3 **conf,int iconf,int create_output_file);
}

#endif
