#include <stdlib.h>
#include <ruby.h>
#include "clips/clips.h"
#include "rbclips.h"
#include "rbrouter.h"
#include "rbenvironment.h"
#include "rbexception.h"
#include "rbrcall.h"

/* Definitions */
VALUE cl_cEnvironment;
VALUE cl_vEnvironments;

/**
 * Will create new environment - this class is a little bit exception,
 * newly created environment is directly inserted into CLIPS - there is no
 * save() method.
 */
VALUE cl_environment_new(VALUE self)
{
  cl_sEnvironmentWrap *wrap = calloc( 1, sizeof(*wrap) );
  
  // Little workaround - CreateEnvironment set the created environment as
  // current one, which is not good for ruby design.
  void *oldenv = GetCurrentEnvironment();

  // Creating new environment
  wrap->ptr = CreateEnvironment();

  // Environment-aware code - put our router into new environment as well
  cl_router_init();

  // Environment-aware code - add our functions to new environment as well
  cl_rcall_define();

  // Taking back the old one
  SetCurrentEnvironment(oldenv);

  VALUE ret = Data_Wrap_Struct(self, NULL, free, wrap);
  rb_obj_call_init(ret, 0, NULL);

  // Saving newly created environment into global list
  rb_ary_push(cl_vEnvironments, ret);

  return ret;
}

/**
 * Basically return string representation of pointer to environment data.
 * Not so usefull at all...
 */
VALUE cl_environment_to_s(VALUE self)
{
  cl_sEnvironmentWrap *wrap = DATA_PTR(self);

  return rb_sprintf("Environment:%lX", (unsigned long)wrap->ptr);
}

/**
 * Returns copy of internal Environment list
 */
VALUE cl_environment_all()
{
  return rb_ary_dup(cl_vEnvironments);
}

/**
 * Set this environment at current one. If environment is not valid
 * than it raise exception.
 */
VALUE cl_environment_set_current(VALUE self)
{
  cl_sEnvironmentWrap *wrap = DATA_PTR(self);

  // TODO: Validity check
  // 1) Is valid?
  // 2) Is not active?

  SetCurrentEnvironment(wrap->ptr);

  return Qtrue;
}

/**
 * Return current environment
 */
VALUE cl_environment_current()
{
  cl_sEnvironmentWrap *wrap = calloc( 1, sizeof(*wrap) );
  
  wrap->ptr = GetCurrentEnvironment();

  VALUE ret = Data_Wrap_Struct(cl_cEnvironment, NULL, free, wrap);
  rb_obj_call_init(ret, 0, NULL);

  return ret;
}

/*
 * Create copy of ruby object - the environment itself will be *not*
 * copyed! <objects after that will describe identical environment>
 */
VALUE cl_environment_clone(VALUE self)
{
  cl_sEnvironmentWrap *selfwrap = DATA_PTR(self);
  cl_sEnvironmentWrap *wrap = calloc( 1, sizeof(*wrap) );
  
  wrap->ptr = selfwrap->ptr;

  VALUE ret = Data_Wrap_Struct(cl_cEnvironment, NULL, free, wrap);
  rb_obj_call_init(ret, 0, NULL);

  return ret;
}

/**
 * Return true or false, base on whether the different environment object
 * describe the same CLIPS environment.
 */
VALUE cl_environment_equal(VALUE a, VALUE b)
{
  CL_EQUAL_CLASS(b, cl_cEnvironment);

  CL_EQUAL_DEFINE_WRAP(cl_sEnvironmentWrap);
  CL_EQUAL_CHECK_PTR;

  return Qtrue;
}

/**
 * Check validity of current class, if the environment that self
 * describe is valid clips environment.
 */
VALUE cl_environment_valid(VALUE self)
{
    return rb_ary_includes(cl_vEnvironments, self);  
}

/*
 * Delete environment from clips. Note that you can delete only not 
 * active environment. It's save to destroy not valid environment
 * (the ona that was destroyed earlier).
 */
VALUE cl_environment_destroy(VALUE self)
{
  cl_sEnvironmentWrap *wrap = DATA_PTR(self);

  // Current environment cannot be deleted
  if(GetCurrentEnvironment() == wrap->ptr)
    return Qfalse;

  if(wrap->ptr && DestroyEnvironment(wrap->ptr))
  {
    // Delete from internal list
    rb_ary_delete(cl_vEnvironments, self);

    wrap->ptr = 0;
    return Qtrue;
  }

  return Qfalse;
}

