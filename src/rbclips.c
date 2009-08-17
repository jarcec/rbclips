#include "rbclips.h"
#include "rbbase.h"
#include "rbenvironment.h"

/* Definitions */
VALUE cl_mClips;

/** Ruby module initialization
 *
 * Create and register all ruby modules and classes
 */
void Init_rbclips()
{
  // Module definition
  cl_mClips = rb_define_module("Clips");

  // Clips::Base
  cl_cBase = rb_define_class_under(cl_mClips, "Base", rb_cObject);

  // Clips::Environment
  cl_cEnvironment = rb_define_class_under(cl_mClips, "Environment", rb_cObject);
  rb_define_singleton_method(cl_cEnvironment, "new", cl_environment_new, 0);
  rb_define_singleton_method(cl_cEnvironment, "all", cl_environment_all, 0);
  rb_define_singleton_method(cl_cEnvironment, "current", cl_environment_current, 0);
  rb_define_method(cl_cEnvironment, "to_s", cl_environment_to_s, 0);
  rb_define_method(cl_cEnvironment, "set_current", cl_environment_set_current, 0);
  rb_define_method(cl_cEnvironment, "clone", cl_environment_clone, 0);
  rb_define_alias(cl_cEnvironment, "dup", "clone");
  rb_define_method(cl_cEnvironment, "==", cl_environment_equal, 1);
  rb_define_alias(cl_cEnvironment, "equal?", "==");
  rb_define_alias(cl_cEnvironment, "eql?", "==");
  rb_define_method(cl_cEnvironment, "valid?", cl_environment_valid, 0);
  rb_define_method(cl_cEnvironment, "destroy!", cl_environment_destroy, 0);

  // Initialization of internal list of environments
  cl_vEnvironments = rb_ary_new();

  // One environments is created by default
  VALUE env = cl_environment_new(cl_cEnvironment);
  cl_environment_set_current(env);

}
