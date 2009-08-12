#include <stdlib.h>
#include "clips/clips.h"
#include "rbclips.h"
#include "rbenvironment.h"

/* Definitions */
VALUE cl_cEnvironment;

/**
 * Will create new environment - this class is a little bit exception,
 * newly created environment is directly inserted into CLIPS - there is no
 * save() method.
 */
VALUE cl_environment_new(VALUE self)
{
  cl_sEnvironmentWrap *wrap = calloc( 1, sizeof(*wrap) );
  
  wrap->ptr = CreateEnvironment();

  VALUE ret = Data_Wrap_Struct(self, NULL, NULL, wrap);
  rb_obj_call_init(ret, 0, NULL);

  return ret;
}

/**
 * Basically return string representation of pointer to environment data.
 * Not so usefull at all...
 */
VALUE cl_environment_to_s(VALUE self)
{
  cl_sEnvironmentWrap *wrap = DATA_PTR(self);

  char tmp[sizeof(void *) * 2 + 1] = {};
  sprintf(tmp, "%X", (unsigned int)wrap->ptr);

  return rb_str_new_cstr(tmp);
}
