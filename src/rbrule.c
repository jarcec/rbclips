#include <stdlib.h>
#include <ruby.h>
#include "clips/clips.h"
#include "rbclips.h"
#include "rbrule.h"
#include "rbgeneric.h"
#include "rbexception.h"
#include "rbbase.h"
#include "rbtemplate.h"

/* Definitions */
VALUE cl_cRule;
VALUE cl_cRuleCreator;

//! Transform array of params representing ordered fact into searchable form
VALUE cl_rule_creator_transform_ordered_fact(int, VALUE *);

//! Transform parameters representing nonordered fact into searchable form
VALUE cl_rule_creator_transform_nonordered_fact(VALUE, VALUE);

//! Foreach method for transforming nonordered fact into searchable form
int cl_rule_creator_transform_nonordered_fact_each(VALUE, VALUE, VALUE);

/**
 * Creating new object - just wrapping structrue
 */
VALUE cl_rule_new(int argc, VALUE *argv, VALUE self)
{
  cl_sRuleWrap *wrap = calloc(1, sizeof(*wrap));

  VALUE ret = Data_Wrap_Struct(self, NULL, free, wrap);
  rb_obj_call_init(ret, argc, argv);

  return ret;
}

/**
 * Constructor of Rule object
 */
VALUE cl_rule_initialize(VALUE self, VALUE name)
{
  if(TYPE(name) != T_STRING && TYPE(name) != T_SYMBOL)
  {
    rb_raise(cl_eArgError, "Clips::Rule#initialize called with wrong argument, required is one String argument, but '%s' is class '%s'", CL_STR(name), CL_STR_CLASS(name));
    return Qnil;
  }

  if( !rb_block_given_p() )
  {
    rb_raise(cl_eArgError, "Clips::Rule#initialize called without block!");
    return Qnil;
  }

  cl_sRuleCreatorWrap *wrap = calloc(1, sizeof(*wrap));
  wrap->ptr = self;

  VALUE creator = Data_Wrap_Struct(cl_cRuleCreator, NULL, free, wrap);
  rb_obj_call_init(creator, 0, NULL);
  rb_yield(creator);

  rb_iv_set(self, "@name", CL_TO_S(name));

  // Rule is too complex to hold values in memory parsed, so current design is
  // to transfer given rule to CLIPS fragment here and keep only this fragment
  VALUE rule = rb_sprintf("(defrule %s", CL_STR(name));
  VALUE side;

  side = rb_iv_get(creator, "@lhs");
  rb_str_catf(rule, " %s", CL_STR(rb_ary_join(side, rb_str_new_cstr(" "))));

  rb_str_cat2(rule, " =>");

  side = rb_iv_get(creator, "@rhs");
  rb_str_catf(rule, " %s", CL_STR(rb_ary_join(side, rb_str_new_cstr(" "))));

  rb_str_cat2(rule, ")");
  rb_iv_set(self, "@to_s", rule);
  return Qtrue;
}

/**
 * Return string representation of a rule, currently return only @to_s
 */
VALUE cl_rule_to_s(VALUE self)
{
  VALUE str = rb_iv_get(self, "@to_s");
  return rb_str_dup(str);
}

/**
 * Saving rule to CLIPS
 */
VALUE cl_rule_save(VALUE self)
{
  // This return false if exception was raised
  VALUE ret = cl_base_insert_command(Qnil, rb_iv_get(self, "@to_s"));

  if(TYPE(ret) != T_FALSE)
  {
    cl_sRuleWrap *wrap = DATA_PTR(self);
    wrap->ptr = FindDefrule( CL_STR(rb_iv_get(self, "@name" )) );
  }

  return Qtrue;
}

/**
 * Remove rule from CLIPS environment
 */
VALUE cl_rule_destroy(VALUE self)
{
  cl_sRuleWrap *wrap = DATA_PTR(self);

  if( !wrap )
  {
    rb_raise(cl_eUseError, "Inner structure not found");
    return Qnil;
  }

  if( !wrap->ptr) return Qfalse;
 
  // Template cannot be in use when deleting
  if( !IsDefruleDeletable(wrap->ptr) )
  {
    rb_raise(cl_eInUseError, "Rule cannot be deleted right now");
    return Qnil;
  }

  // Return
  VALUE ret = Qfalse;

  if( Undefrule(wrap->ptr) )
    ret = Qtrue;

  // It doesn't matter how the undeftemplate ends - the template is not in CLIPS anyway...
  wrap->ptr = NULL;

  return ret;
}

/**
 * Constructor checks if this object was created by Clips::Rule. If not it raise an
 * exception, bacause it's prohibited.
 */
VALUE cl_rule_creator_initialize(VALUE self)
{
  cl_sRuleCreatorWrap *wrap = DATA_PTR(self);
  if( !wrap || !wrap->ptr )
  {
    rb_raise(cl_eUseError, "Direct creation of Clips::Rule::Creator is prohibited.");
    return Qnil;
  }

  // Creating internal variables
  rb_iv_set(self, "@lhs", rb_ary_new());
  rb_iv_set(self, "@rhs", rb_ary_new());

  return Qtrue;
}

/**
 * Declare searching pattern of the rule (LHS)
 */
VALUE cl_rule_creator_pattern(int argc, VALUE *argv, VALUE self)
{
  VALUE lhs = rb_iv_get(self, "@lhs");

  if(argc == 0)
  {
    rb_raise(cl_eArgError, "Calling Clips::Rule::Creator#pattern without arguments is prohibited. See manual for all allowed posibilities.");
    return Qnil;
  }

  if(argc == 2 && rb_obj_class(argv[0]) == cl_cTemplate && TYPE(argv[1]) == T_HASH)
  {
    rb_ary_push(lhs, cl_rule_creator_transform_nonordered_fact(argv[0], argv[1]) );
    return Qtrue;
  }

  if(argc > 1 && (TYPE(argv[0]) == T_STRING || TYPE(argv[0]) == T_SYMBOL))
  {
    // Assume we're searching for ordered fact
    rb_ary_push(lhs, cl_rule_creator_transform_ordered_fact(argc, argv) );
    return Qtrue;
  }

  if(argc == 1 && TYPE(argv[0]) == T_STRING)
  {
    rb_ary_push(lhs, argv[0]);
    return Qtrue;
  }

  rb_raise(cl_eArgError, "Calling Clips::Rule::Creator#pattern with unknown parameter set. See manual for all allowed posibilities.");
  return Qnil;
}

/**
 * Declare searching and destroying pattern
 */
VALUE cl_rule_creator_retract(int argc, VALUE *argv, VALUE self)
{
  cl_sRuleCreatorWrap *wrap = DATA_PTR(self);
  if( !wrap )
  {
    rb_raise(cl_eUseError, "Inner structure not found");
    return Qnil;
  }

  VALUE lhs = rb_iv_get(self, "@lhs");
  VALUE rhs = rb_iv_get(self, "@rhs");

  if(argc == 0)
  {  
    rb_raise(cl_eArgError, "Calling Clips::Rule::Creator#retract without arguments is prohibited. See manual for all allowed posibilities.");
    return Qnil;
  }

  if(argc == 2 && rb_obj_class(argv[0]) == cl_cTemplate && TYPE(argv[1]) == T_HASH)
  {
    VALUE pom = rb_sprintf("?rbclips-%u <- %s", wrap->counter, CL_STR(cl_rule_creator_transform_nonordered_fact(argv[0], argv[1])));

    rb_ary_push(lhs, pom);
    rb_ary_push(rhs, rb_sprintf("(retract ?rbclips-%u)", wrap->counter));

    wrap->counter++;
    return Qtrue;
    return Qtrue;
  }

  if(argc > 1 && (TYPE(argv[0]) == T_STRING || TYPE(argv[0]) == T_SYMBOL))
  {
    // Assume we're searching for ordered fact
    VALUE pom = rb_sprintf("?rbclips-%u <- %s", wrap->counter, CL_STR(cl_rule_creator_transform_ordered_fact(argc, argv)));

    rb_ary_push(lhs, pom);
    rb_ary_push(rhs, rb_sprintf("(retract ?rbclips-%u)", wrap->counter));
    
    wrap->counter++;
    return Qtrue;
  }

  rb_raise(cl_eArgError, "Calling Clips::Rule::Creator#retract with unknown parameter set. See manual for all allowed posibilities.");
  return Qnil;
}

/**
 * Direct inserting of CLIPS fragment into right hand side of a rule
 */
VALUE cl_rule_creator_rhs(VALUE self, VALUE str)
{
  if(TYPE(str) != T_STRING)
  {
    rb_raise(cl_eArgError, "Calling Clips::Rule::Creator#rhs expect string parameter, but given '%s' have class '%s'.", CL_STR(str), CL_STR_CLASS(str));
    return Qnil;
  }
  
  VALUE rhs = rb_iv_get(self, "@rhs");
  rb_ary_push(rhs, str);
  
  return Qtrue;
}

/**
 * Helper: Transform given array of parameters into searchable pattern
 * for left hand side of the rule definition.
 */
VALUE cl_rule_creator_transform_ordered_fact(int argc, VALUE *argv)
{
  VALUE ret = rb_sprintf("(%s", CL_STR(argv[0]));

  int i;
  for(i = 1; i < argc; i++)
    rb_str_catf(ret, " %s", rb_generic_slot_value(argv[i]));

  rb_str_cat2(ret, ")");
  return ret;
}

/**
 * Helper: Transform two arguments into searchable pattern
 * for left hand side of rule definition.
 */
VALUE cl_rule_creator_transform_nonordered_fact(VALUE template, VALUE slots)
{
  VALUE t = rb_iv_get(template, "@name");
  VALUE tslots = rb_iv_get(template, "@slots");
  VALUE ret = rb_sprintf("(%s", CL_STR(t));

  VALUE ary = rb_ary_new();
  rb_ary_push(ary, tslots);
  rb_ary_push(ary, ret);

  rb_hash_foreach(slots, cl_rule_creator_transform_nonordered_fact_each, ary);

  rb_str_cat2(ret, ")");
  return ret;
}

/**
 * Helper: Foreach function for transforming
 */
int cl_rule_creator_transform_nonordered_fact_each(VALUE key, VALUE value, VALUE ar)
{
  VALUE tslots =    rb_ary_entry(ar, 0);
  VALUE ret =       rb_ary_entry(ar, 1);

  VALUE c  = rb_hash_lookup(tslots, key);
  if(NIL_P(c))
  {
    rb_raise(cl_eArgError, "Given slot '%s' don't exists.", CL_STR(key));
    return ST_STOP;
  }

  rb_str_catf(ret, " (%s %s)", CL_STR(key), rb_generic_slot_value(value));
  return ST_CONTINUE;
}
