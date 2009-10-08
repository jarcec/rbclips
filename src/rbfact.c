#include <stdlib.h>
#include <ruby.h>
#include "clips/clips.h"
#include "rbclips.h"
#include "rbfact.h"
#include "rbtemplate.h"
#include "rbconstraint.h"
#include "rbexception.h"
#include "rbbase.h"
#include "rbgeneric.h"

/* Definitions */
VALUE cl_cFact;

//! Create ordered fact
VALUE cl_fact_initialize_ordered(VALUE, VALUE, VALUE);

//! Create nonordered fact
VALUE cl_fact_initialize_nonordered(VALUE, VALUE, VALUE);

//! to_s method for ordered fact
VALUE cl_fact_to_s_ordered(VALUE);

//! to_s method for nonordered fact
VALUE cl_fact_to_s_nonordered(VALUE);

//! Foreach method for creating nonordered fact
int cl_fact_initialize_nonordered_each(VALUE, VALUE, VALUE);

/**
 * Creating new object - wrap struct
 */
VALUE cl_fact_new(int argc, VALUE *argv, VALUE self)
{
  cl_sFactWrap *wrap = calloc(1, sizeof(*wrap));

  VALUE ret = Data_Wrap_Struct(self, NULL, free, wrap);
  rb_obj_call_init(ret, argc, argv);

  return ret;
}

/**
 * Constructor - initialize
 */
VALUE cl_fact_initialize(VALUE self, VALUE first, VALUE second)
{
  if(TYPE(first) == T_STRING && TYPE(second) == T_ARRAY)
  {
    return cl_fact_initialize_ordered(self, first, second);
  }

  if(rb_obj_class(first) == cl_cTemplate && TYPE(second) == T_HASH)
  {
    return cl_fact_initialize_nonordered(self, first, second);
  }

  rb_raise(cl_eArgError, "Clips::Fact#initialize expected two possibility of arguments String with Array or Template with Hash");
  return Qnil;
}

/**
 * Initialize an ordered fact
 */
VALUE cl_fact_initialize_ordered(VALUE self, VALUE first, VALUE second)
{
  if( !cl_generic_check_clips_symbol(first) )
  {
    rb_raise(cl_eArgError, "Clips::Fact#initialize '%s' is not valid CLIPS symbol.", CL_STR(first));
    return Qnil;
  }

  rb_iv_set(self, "@template", first);
  rb_iv_set(self, "@slots", second);

  rb_define_singleton_method(self, "slots", cl_fact_slots, 0);

  return Qtrue;
}

/**
 * Return slot list in case of ordered fact
 */
VALUE cl_fact_slots(VALUE self)
{
  VALUE slots = rb_iv_get(self, "@slots");
  return rb_ary_dup(slots);
}

/**
 * Initialize a nonordered fact
 */
VALUE cl_fact_initialize_nonordered(VALUE self, VALUE first, VALUE second)
{
  if( TYPE( rb_funcall(first, cl_vIds.saved, 0) ) != T_TRUE)
  {
    rb_raise(cl_eArgError, "Clips::Fact#initialize template is not saved!.");
    return ST_STOP;
  }

  rb_iv_set(self, "@template", first);
  rb_iv_set(self, "@slots", rb_hash_new());

  // Check if slot exists
  rb_hash_foreach(second, cl_fact_initialize_nonordered_each, self);

  return Qtrue;
}

/**
 * Set slots from given hash (maybe more check later) (constraints?)
 */
int cl_fact_initialize_nonordered_each(VALUE key, VALUE value, VALUE self)
{
  if(TYPE(key) != T_STRING && TYPE(key) != T_SYMBOL)
  {
    rb_raise(cl_eArgError, "Clips::Fact#initialize slot assignement accept only strings or symbols as keys but '%s' have class '%s'.", CL_STR(key), CL_STR_CLASS(key));
    return ST_STOP;
  }

  VALUE template = rb_iv_get(self, "@template");
  VALUE slots    = rb_iv_get(self, "@slots");
  VALUE tslots   = rb_iv_get(template, "@slots");

  // Transfer type if necessary
  if(TYPE(key) == T_STRING)
    key =  rb_funcall(key, cl_vIds.to_sym, 0);

  // Does the slot exists in template?
  VALUE c  = rb_hash_lookup(tslots, key);
  if( NIL_P(c) )
  {
    rb_raise(cl_eArgError, "Clips::Fact#initialize template don't have slot '%s'.", CL_STR(key));
    return ST_STOP;
  }

  // It shouldn't be defined already
  c  = rb_hash_lookup(slots, key);
  if( !NIL_P(c) )
  {
    rb_raise(cl_eArgError, "Clips::Fact#initialize slot '%s' already filled up.", CL_STR(key));
    return ST_STOP;
  }

  rb_hash_aset(slots, key, value);

  return ST_CONTINUE;
}

/**
 * Return string reprezentation
 */
VALUE cl_fact_to_s(VALUE self)
{
  VALUE ord = rb_funcall(self, cl_vIds.ordered, 0);

  if(TYPE(ord) == T_TRUE) return cl_fact_to_s_ordered(self);

  return cl_fact_to_s_nonordered(self);
}

/**
 * to_s: Ordered variant
 */
VALUE cl_fact_to_s_ordered(VALUE self)
{
  VALUE ret = rb_str_new2("(");
  VALUE template = rb_iv_get(self, "@template");
  VALUE slots = rb_iv_get(self, "@slots");

  rb_str_cat2(ret, CL_STR(template));

  // Non ordered slot
  long len = RARRAY_LEN(slots);
  int i;
  for(i = 0; i < len; i++)
    rb_str_catf(ret, " %s", CL_STR(rb_ary_entry(slots, i)) );

  rb_str_cat2(ret, ")");
  return ret;
}

/**
 * Duplicate this object
 */
VALUE cl_fact_clone(VALUE self)
{
  CL_UPDATE(self);

  cl_sFactWrap *selfwrap = DATA_PTR(self);
  cl_sFactWrap *wrap = calloc( 1, sizeof(*wrap) );
  
  wrap->ptr = selfwrap->ptr;

  VALUE ret = Data_Wrap_Struct(cl_cFact, NULL, free, wrap);

  VALUE argv[2];
  argv[0] = rb_iv_get(self, "@template");
  argv[1] = rb_iv_get(self, "@slots");

  rb_obj_call_init(ret, 2, argv);

  return ret;
}

/**
 * Represent this two objects same CLIPS fact?
 */
VALUE cl_fact_equal(VALUE a, VALUE b)
{
  CL_UPDATE(a);
  CL_UPDATE(b);

  CL_EQUAL_DEFINE;
  
  CL_EQUAL_CHECK_IV("@template");
  CL_EQUAL_CHECK_IV("@slots");
  
  CL_EQUAL_DEFINE_WRAP(cl_sFactWrap);
  CL_EQUAL_CHECK_PTR;

  return Qtrue;
}

/**
 * Save fact to CLIPS environment
 */
VALUE cl_fact_save(VALUE self)
{
  CL_UPDATE(self);

  cl_sFactWrap *wrap = DATA_PTR(self);
  if( !wrap )
  {
      rb_raise(cl_eUseError, "Oops, wrap structure don't exists!");
      return Qnil;
  }

  // Valid?
  if ( !FactExistp(wrap->ptr) )
   wrap->ptr = NULL;

  // Already saved?
  if(wrap->ptr) return Qfalse;

  // Saving
  wrap->ptr = AssertString( CL_STR(self) );

  return Qtrue;
}

/**
 * Update inner structures from CLIPS environment
 */
VALUE cl_fact_update(VALUE self)
{
  cl_sFactWrap *wrap = DATA_PTR(self);
  if( !wrap )
  {
      rb_raise(cl_eUseError, "Oops, wrap structure don't exists!");
      return Qnil;
  }

  // Valid?
  if ( !FactExistp(wrap->ptr) )
   wrap->ptr = NULL;

  return Qtrue;
}

/**
 * Remove fact from CLIPS environment
 */
VALUE cl_fact_destroy(VALUE self)
{
  CL_UPDATE(self);

  cl_sFactWrap *wrap = DATA_PTR(self);
  if( !wrap )
  {
      rb_raise(cl_eUseError, "Oops, wrap structure don't exists!");
      return Qnil;
  }

  // if NULL return (don't rectract something that is not saved)
  if( !wrap->ptr ) return Qfalse;

  if( Retract(wrap->ptr) ) return Qtrue;

  return Qfalse;
}

/**
 * Is this fact ordered or non ordered?
 */
VALUE cl_fact_ordered(VALUE self)
{
  VALUE name = rb_iv_get(self, "@template");

  if(TYPE(name) == T_STRING) return Qtrue;

  return Qfalse;
}
