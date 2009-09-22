#include "ruby.h"
#include "rbclips.h"
#include "rbbase.h"
#include "rbenvironment.h"
#include "rbrouter.h"
#include "rbexception.h"
#include "rbconstraint.h"

/* Definitions */
VALUE cl_mClips;
cl_sIds cl_vIds;

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
  rb_define_singleton_method(cl_cBase, "insert_command", cl_base_insert_command, 1);

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

  // Clips::Constraint
  cl_cConstraint = rb_define_class_under(cl_mClips, "Constraint", rb_cObject);
  rb_define_singleton_method(cl_cConstraint, "new", cl_constraint_new, -1);

  // Exception classes
  cl_eException = rb_define_class_under(cl_mClips, "Exception", rb_eException);
  cl_eArgError = rb_define_class_under(cl_mClips, "ArgumentError", cl_eException);

  // Initialization of internal list of environments
  cl_vEnvironments = rb_ary_new();

  // One environments is created by default
  VALUE env = cl_environment_new(cl_cEnvironment);
  cl_environment_set_current(env);

  // Creating symbol list
  cl_vIds.any                 = rb_intern("any");
  cl_vIds.symbol              = rb_intern("symbol");
  cl_vIds.string              = rb_intern("string");
  cl_vIds.lexeme              = rb_intern("lexeme");
  cl_vIds.integer             = rb_intern("integer");
  cl_vIds.float_              = rb_intern("float");
  cl_vIds.number              = rb_intern("number");
  cl_vIds.instance_name       = rb_intern("instance_name");
  cl_vIds.instance_address    = rb_intern("instance_address");
  cl_vIds.instance            = rb_intern("instance");
  cl_vIds.external_address    = rb_intern("external_address");
  cl_vIds.fact_address        = rb_intern("fact_address");

  // Initialization of routering system
  cl_router_init();
}
