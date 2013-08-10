#ifndef _BGQ_INTRINSIC_DECLARE
#define _BGQ_INTRINSIC_DECLARE

#include "../src/base/macros.h"

#define DECLARE_REG_BI_COMPLEX(A) reg_bi_complex A

#define DECLARE_REG_BI_COLOR(A)			\
  DECLARE_REG_BI_COMPLEX(NAME2(A,c0));		\
  DECLARE_REG_BI_COMPLEX(NAME2(A,c1));		\
  DECLARE_REG_BI_COMPLEX(NAME2(A,c2));		\

#define DECLARE_REG_BI_SPINCOLOR(A)		\
  DECLARE_REG_BI_COLOR(NAME2(A,s0));		\
  DECLARE_REG_BI_COLOR(NAME2(A,s1));		\
  DECLARE_REG_BI_COLOR(NAME2(A,s2));		\
  DECLARE_REG_BI_COLOR(NAME2(A,s3));		\

#define DECLARE_REG_BI_HALFSPIN(A)		\
  DECLARE_REG_BI_COMPLEX(NAME2(A,s0));		\
  DECLARE_REG_BI_COMPLEX(NAME2(A,s1));		\

#define DECLARE_REG_BI_HALFSPINCOLOR(A)		\
  DECLARE_REG_BI_COLOR(NAME2(A,s0));		\
  DECLARE_REG_BI_COLOR(NAME2(A,s1));		\

#define DECLARE_REG_BI_SU3(A)			\
  DECLARE_REG_BI_COMPLEX(NAME2(A,c00));		\
  DECLARE_REG_BI_COMPLEX(NAME2(A,c01));		\
  DECLARE_REG_BI_COMPLEX(NAME2(A,c02));		\
  DECLARE_REG_BI_COMPLEX(NAME2(A,c10));		\
  DECLARE_REG_BI_COMPLEX(NAME2(A,c11));		\
  DECLARE_REG_BI_COMPLEX(NAME2(A,c12));		\
  DECLARE_REG_BI_COMPLEX(NAME2(A,c20));		\
  DECLARE_REG_BI_COMPLEX(NAME2(A,c21));		\
  DECLARE_REG_BI_COMPLEX(NAME2(A,c22));		\

#endif