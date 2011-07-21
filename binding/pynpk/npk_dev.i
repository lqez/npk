%module npk
%{
#include <npk_conf.h>
#include <npk_base.h>
#include <npk_error.h>
#include <npk.h>
#include <npk_dev.h>
%}
%include "swig.i"
%include <npk.h>
%include <npk_dev.h>
