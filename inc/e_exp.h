#ifndef E_EXP_H
#define E_EXP_H

#include "sizedtypes.h"

f64 __ieee754_exp(f64 x);

#define e_exp(x) __ieee754_exp(x)

#endif
