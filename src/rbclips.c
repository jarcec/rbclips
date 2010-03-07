#include <ruby.h>
#include "rbclips.h"
#include "rbbase.h"
#include "rbenvironment.h"
#include "rbrouter.h"
#include "rbexception.h"
#include "rbconstraint.h"
#include "rbtemplate.h"
#include "rbfact.h"
#include "rbrule.h"
#include "rbrcall.h"

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
  rb_define_singleton_method(cl_cBase, "run", cl_base_run, -1);
  rb_define_singleton_method(cl_cBase, "reset", cl_base_reset, 0);
  rb_define_singleton_method(cl_cBase, "clear", cl_base_clear, 0);

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
  rb_define_method(cl_cConstraint, "==", cl_constraint_equal, 1);
  rb_define_alias(cl_cConstraint, "equal?", "==");
  rb_define_alias(cl_cConstraint, "eql?", "==");

  // Clips::Constraint::Creator
  cl_cConstraintCreator = rb_define_class_under(cl_cConstraint, "Creator", rb_cObject);
  rb_define_method(cl_cConstraintCreator, "initialize", cl_constraint_creator_initialize, 0);
  rb_define_method(cl_cConstraintCreator, "type", cl_constraint_creator_type, 1);
  rb_define_method(cl_cConstraintCreator, "values", cl_constraint_creator_values, 1);
  rb_define_method(cl_cConstraintCreator, "cardinality", cl_constraint_creator_cardinality, 1);
  rb_define_method(cl_cConstraintCreator, "range", cl_constraint_creator_range, 1);

  // Clips::Template
  cl_cTemplate = rb_define_class_under(cl_mClips, "Template", rb_cObject);
  rb_define_singleton_method(cl_cTemplate, "new", cl_template_new, -1);
  rb_define_singleton_method(cl_cTemplate, "load", cl_template_load, 1);
  rb_define_singleton_method(cl_cTemplate, "all", cl_template_all, 0);
  rb_define_method(cl_cTemplate, "initialize", cl_template_initialize, -1);
  rb_define_method(cl_cTemplate, "to_s", cl_template_to_s, 0);
  rb_define_method(cl_cTemplate, "clone", cl_template_clone, 0);
  rb_define_alias(cl_cTemplate, "dup", "clone");
  rb_define_method(cl_cTemplate, "==", cl_template_equal, 1);
  rb_define_alias(cl_cTemplate, "equal?", "==");
  rb_define_alias(cl_cTemplate, "eql?", "==");
  rb_define_method(cl_cTemplate, "save", cl_template_save, 0);
  rb_define_method(cl_cTemplate, "saved?", cl_template_saved, 0);
  rb_define_method(cl_cTemplate, "destroy!", cl_template_destroy, 0);
  rb_define_method(cl_cTemplate, "update", cl_template_update, 0);

  // Clips::Template::Creator
  cl_cTemplateCreator = rb_define_class_under(cl_cTemplate, "Creator", rb_cObject);
  rb_define_method(cl_cTemplateCreator, "initialize", cl_template_creator_initialize, 0);
  rb_define_method(cl_cTemplateCreator, "slot", cl_template_creator_slot, -1);

  // Clips::Fact
  cl_cFact = rb_define_class_under(cl_mClips, "Fact", rb_cObject);
  rb_define_singleton_method(cl_cFact, "new", cl_fact_new, -1);
  rb_define_singleton_method(cl_cFact, "all", cl_fact_all, 0);
  rb_define_method(cl_cFact, "initialize", cl_fact_initialize, 2);
  rb_define_method(cl_cFact, "to_s", cl_fact_to_s, 0);
  rb_define_method(cl_cFact, "clone", cl_fact_clone, 0);
  rb_define_alias(cl_cFact, "dup", "clone");
  rb_define_method(cl_cFact, "==", cl_fact_equal, 1);
  rb_define_alias(cl_cFact, "equal?", "==");
  rb_define_alias(cl_cFact, "eql?", "==");
  rb_define_method(cl_cFact, "save", cl_fact_save, 0);
  rb_define_method(cl_cFact, "destroy!", cl_fact_destroy, 0);
  rb_define_method(cl_cFact, "ordered?", cl_fact_ordered, 0);
  rb_define_method(cl_cFact, "update", cl_fact_update, 0);

  // Clips::Rule
  cl_cRule = rb_define_class_under(cl_mClips, "Rule", rb_cObject);
  rb_define_singleton_method(cl_cRule, "new", cl_rule_new, -1);
  rb_define_singleton_method(cl_cRule, "load", cl_rule_load, 1);
  rb_define_singleton_method(cl_cRule, "all", cl_rule_all, 0);
  rb_define_method(cl_cRule, "initialize", cl_rule_initialize, 1);
  rb_define_method(cl_cRule, "to_s", cl_rule_to_s, 0);
  rb_define_method(cl_cRule, "save", cl_rule_save, 0);
  rb_define_method(cl_cRule, "destroy!", cl_rule_destroy, 0);
  rb_define_method(cl_cRule, "update", cl_rule_update, 0);
  rb_define_method(cl_cRule, "==", cl_rule_equal, 1);
  rb_define_alias(cl_cRule, "equal?", "==");
  rb_define_alias(cl_cRule, "eql?", "==");

  // Clips::Rule::Creator
  cl_cRuleCreator = rb_define_class_under(cl_cRule, "Creator", rb_cObject);
  rb_define_method(cl_cRuleCreator, "initialize", cl_rule_creator_initialize, 0);
  rb_define_method(cl_cRuleCreator, "pattern", cl_rule_creator_pattern, -1);
  rb_define_method(cl_cRuleCreator, "retract", cl_rule_creator_retract, -1);
  rb_define_method(cl_cRuleCreator, "rhs", cl_rule_creator_rhs, 1);
  rb_define_method(cl_cRuleCreator, "rcall", cl_rule_creator_rcall, -1);
  rb_define_method(cl_cRuleCreator, "or", cl_rule_creator_or, 0);
  rb_define_method(cl_cRuleCreator, "and", cl_rule_creator_and, 0);
  rb_define_method(cl_cRuleCreator, "not", cl_rule_creator_not, 0);

  // Exception classes
  cl_eException = rb_define_class_under(cl_mClips, "Exception", rb_eException);
  cl_eArgError = rb_define_class_under(cl_mClips, "ArgumentError", cl_eException);
  cl_eUseError = rb_define_class_under(cl_mClips, "UsageError", cl_eException);
  cl_eInUseError = rb_define_class_under(cl_mClips, "InUseError", cl_eException);
  cl_eInternError = rb_define_class_under(cl_mClips, "InternalError", cl_eException);
  cl_eNotImplError = rb_define_class_under(cl_mClips, "NotImplementedError", cl_eException);

  // Initialization of internal list of environments
  cl_vEnvironments = rb_ary_new();
  rb_global_variable(&cl_vEnvironments);

  // One environments is created by befault
  VALUE env = cl_environment_new(cl_cEnvironment);
  cl_environment_set_current(env);

  // Creating symbol list
  cl_vIds.any                 = rb_intern("any");
  cl_vIds.unknown             = rb_intern("unknown");
  cl_vIds.one                 = rb_intern("one");
  cl_vIds.all                 = rb_intern("all");
  cl_vIds.to_s                = rb_intern("to_s");
  cl_vIds.to_sym              = rb_intern("to_sym");
  cl_vIds.begin               = rb_intern("begin");
  cl_vIds.end                 = rb_intern("end");
  cl_vIds.clear               = rb_intern("clear");
  cl_vIds.size                = rb_intern("size");
  cl_vIds.update              = rb_intern("update");
  cl_vIds.eqq                 = rb_intern("===");
  cl_vIds.ordered             = rb_intern("ordered?");
  cl_vIds.saved               = rb_intern("saved?");
  cl_vIds.clone               = rb_intern("clone");
  cl_vIds.load                = rb_intern("load");
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
}
