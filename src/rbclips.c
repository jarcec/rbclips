#include <ruby.h>
#include "rbclips.h"
#include "rbbase.h"
#include "rbenvironment.h"
#include "rbrouter.h"
#include "rbexception.h"
#include "rbconstraint.h"
#include "rbtemplate.h"

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
  rb_define_method(cl_cConstraint, "initialize", cl_constraint_initialize, -1);
  rb_define_method(cl_cConstraint, "to_s", cl_constraint_to_s, 0);

  // Clips::Constraint::Creator
  cl_cConstraintCreator = rb_define_class_under(cl_cConstraint, "Creator", rb_cObject);
  rb_define_method(cl_cConstraintCreator, "initialize", cl_constraint_creator_initialize, 0);
  rb_define_method(cl_cConstraintCreator, "type", cl_constraint_creator_type, 1);
  rb_define_method(cl_cConstraintCreator, "values", cl_constraint_creator_values, 1);
  rb_define_method(cl_cConstraintCreator, "cardinality", cl_constraint_creator_cardinality, 1);
  rb_define_method(cl_cConstraintCreator, "range", cl_constraint_creator_range, 1);

  // Clips::Template
  cl_cTemplate = rb_define_class_under(cl_mClips, "Template", rb_cObject);
  rb_define_method(cl_cTemplate, "initialize", cl_template_initialize, -1);
  rb_define_method(cl_cTemplate, "to_s", cl_template_to_s, 0);

  // Exception classes
  cl_eException = rb_define_class_under(cl_mClips, "Exception", rb_eException);
  cl_eArgError = rb_define_class_under(cl_mClips, "ArgumentError", cl_eException);
  cl_eUseError = rb_define_class_under(cl_mClips, "UsageError", cl_eException);

  // Initialization of internal list of environments
  cl_vEnvironments = rb_ary_new();
  rb_global_variable(&cl_vEnvironments);

  // One environments is created by befault
  VALUE env = cl_environment_new(cl_cEnvironment);
  cl_environment_set_current(env);

  // Creating symbol list
  cl_vIds.any                 = rb_intern("any");
  cl_vIds.unknown             = rb_intern("unknown");
  cl_vIds.to_s                = rb_intern("to_s");
  cl_vIds.to_sym              = rb_intern("to_sym");
  cl_vIds.begin               = rb_intern("begin");
  cl_vIds.end                 = rb_intern("end");
  cl_vIds.clear               = rb_intern("clear");
  cl_vIds.size                = rb_intern("size");
  cl_vIds.type                = rb_intern("type");
  cl_vIds.values              = rb_intern("values");
  cl_vIds.range               = rb_intern("range");
  cl_vIds.cardinality         = rb_intern("cardinality");
  cl_vIds.name                = rb_intern("name");
  cl_vIds.slots               = rb_intern("slots");
  cl_vIds.multislot           = rb_intern("multislot");
  cl_vIds.default_            = rb_intern("default");
  cl_vIds.default_dynamic     = rb_intern("default_dynamic");
  cl_vIds.constraint          = rb_intern("constraint");
  cl_vIds.none                = rb_intern("none");
  cl_vIds.derive              = rb_intern("derive");
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
