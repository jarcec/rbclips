#include <stdlib.h>
#include <ruby.h>
#include "clips/clips.h"
#include "rbclips.h"
#include "rbrule.h"
#include "rbgeneric.h"
#include "rbexception.h"
#include "rbbase.h"

/* Definitions */
VALUE cl_cRule;
VALUE cl_cRuleCreator;

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
  rb_yield(creator);

  return Qtrue;
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

  return Qtrue;
}
