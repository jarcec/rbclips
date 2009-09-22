#ifndef _RBCLIPS_H_
#define _RBCLIPS_H_

#include "ruby.h"

//! rbClips main module
extern VALUE cl_mClips;

//! Holds symbols that are used in many places across rbclips
struct _cl_sIds
{
  // General
  ID any;

  // CLIPS build-in types
  ID symbol;
  ID string;
  ID lexeme;
  ID integer;
  ID float_;
  ID number;
  ID instance_name;
  ID instance_address;
  ID instance;
  ID external_address;
  ID fact_address;
};
typedef struct _cl_sIds cl_sIds;
extern cl_sIds cl_vIds;

#endif // _RBCLIPS_H_
