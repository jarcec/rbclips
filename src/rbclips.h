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
  ID unknown;

  // Ruby methods
  ID to_s;
  ID begin;
  ID end;

  // Mostly hash keys
  ID type;
  ID values;
  ID range;
  ID cardinality;

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

//! Internal list of general used IDs
extern cl_sIds cl_vIds;

// Some C "magic"
#ifndef false
# define false 0
#endif

#ifndef true
# define true 1
#endif

#endif // _RBCLIPS_H_
