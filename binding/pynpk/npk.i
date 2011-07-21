%module npk
%include typemaps.i
%{
#include <npk_conf.h>
#include <npk_base.h>
#include <npk_error.h>
#include "npk_binding.h"
%}
%include "npk_binding.h"
