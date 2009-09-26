#include <stdlib.h>
#include <ruby.h>
#include "clips/clips.h"
#include "rbclips.h"
#include "rbtemplate.h"
#include "rbconstraint.h"
#include "rbenvironment.h"
#include "rbexception.h"

/* Definitions */
VALUE cl_cTemplate;

//! Creating new Constraint object using block
VALUE cl_template_initialize_block(VALUE, VALUE);

//! Creating new Constraint object using hash
VALUE cl_template_initialize_hash(VALUE, VALUE);

//! Go throw configuring hash with slots
int cl_template_initialize_hash_each(VALUE, VALUE, VALUE);

//! Set up given variable (if it's valid)
int cl_template_initialize_check_variable(VALUE, VALUE, VALUE);

/**
 * Constructor that accept hash or name with config block
 */
VALUE cl_template_initialize(int argc, VALUE *argv, VALUE self)
{
  if(argc == 1 && TYPE(argv[0]) == T_STRING && rb_block_given_p())
  {
    return cl_template_initialize_block(self, argv[0]);
  }

  if(argc == 1 && TYPE(argv[0]) == T_HASH)
  {
    return cl_template_initialize_hash(self, argv[0]);
  }

  rb_raise(cl_eArgError, "Clips::Template#initialize called with wrong arguments, required is one Hash argument or one String argument with block.");
  return Qnil;
}

/**
 * Build template from configuring hash
 */
VALUE cl_template_initialize_hash(VALUE self, VALUE hash)
{
  // Configuring hash is pretty straightforward, there have to be two keys :name and :slots
  long size   = NUM2LONG(rb_funcall(hash, cl_vIds.size, 0));
  VALUE name  = rb_hash_lookup(hash, ID2SYM(cl_vIds.name) );
  VALUE slots = rb_hash_lookup(hash, ID2SYM(cl_vIds.slots) );

  if( ! (size == 2 && !NIL_P(name) && !NIL_P(slots)) )
  {
    rb_raise(cl_eArgError, "Clips::Template#intialize called with hash need hash with two compulsory keys :name and :slots.");
    return Qnil;
  }

  if( TYPE(name) != T_STRING)
  {
    rb_raise(cl_eArgError, "Clips::Template#intialize :name needs an String as parametr but '%s' of class '%s' was given.", CL_STR(name), CL_STR_CLASS(name));
    return Qnil;
  }

  if( TYPE(slots) != T_ARRAY && TYPE(slots) != T_HASH)
  {
    rb_raise(cl_eArgError, "Clips::Template#intialize :slots needs an Hash or Array as parametr but '%s' of class '%s' was given.", CL_STR(slots), CL_STR_CLASS(slots));
    return Qnil;
  }

  // Parsing and creating
  VALUE s = rb_hash_new();

  if(TYPE(slots) == T_ARRAY)
  {
    long len = RARRAY_LEN(slots);
    int i;
    for(i = 0; i < len; i++)
    {
      VALUE entry = rb_ary_entry(slots, i);
      if(TYPE(entry) != T_SYMBOL && TYPE(entry) != T_STRING)
      {
        rb_raise(cl_eArgError, "Clips::Template#initialize :slots key accept Array of strings or symbols but '%s' have class '%s'.", CL_STR(entry), CL_STR_CLASS(entry));
        return Qnil;
      }

      if(TYPE(entry) == T_STRING)
        entry =  rb_funcall(entry, cl_vIds.to_sym, 0);

      rb_hash_aset(s, entry, Qnil);
    }
  }

  if(TYPE(slots) == T_HASH)
  {
    rb_hash_foreach(slots, cl_template_initialize_hash_each, s);
  }

  // Saving to class
  rb_iv_set(self, "@name", name);
  rb_iv_set(self, "@slots", s);

  return Qtrue;
}

/**
 * Build template from block configuring block
 */
VALUE cl_template_initialize_block(VALUE self, VALUE name)
{
  return Qnil;
}

/**
 * First line of configuring hash :slot_name => {:option1 => x, :option2=> y}
 */
int cl_template_initialize_hash_each(VALUE key, VALUE value, VALUE s)
{
  if(TYPE(key) != T_SYMBOL && TYPE(key) != T_STRING)
  {
    rb_raise(cl_eArgError, "Clips::Template#initialize :slots key accept Hash with strings or symbols keys but '%s' have class '%s'.", CL_STR(key), CL_STR_CLASS(key));
    return ST_STOP;
  }
  if(TYPE(value) != T_HASH)
  {
    rb_raise(cl_eArgError, "Clips::Template#initialize :slots key request another hash as an value but '%s' have class '%s'.", CL_STR(value), CL_STR_CLASS(value));
    return ST_STOP;
  }

  // Transfer type if necessary
  if(TYPE(key) == T_STRING)
    key =  rb_funcall(key, cl_vIds.to_sym, 0);

  // Check if it's valid slot configuration
  rb_hash_foreach(value, cl_template_initialize_check_variable, s);

  // Save this slot, it's valid
  rb_hash_aset(s, key, value);

  // C'est tout
  return ST_CONTINUE;
}

/**
 * Second line of configuring hash, parse and check options for one particular slot
 */
int cl_template_initialize_check_variable(VALUE key, VALUE value, VALUE s)
{
  // Check argument type
  if(TYPE(key) != T_SYMBOL && TYPE(key) != T_STRING)
  {
    rb_raise(cl_eArgError, "Key '%s' have class '%s' but expected was Symbol or String.", CL_STR(key), CL_STR_CLASS(value) );
    return ST_STOP;
  }

  // Transfer it to ID
  ID sym_key = rb_to_id(key);

  if(sym_key == cl_vIds.multislot)
  {
    if(TYPE(value) != T_TRUE && TYPE(value) != T_FALSE)
    {
      rb_raise(cl_eArgError, "Clips::Template#initialize :multislot key request true or false as value but '%s' have class '%s'.", CL_STR(value), CL_STR_CLASS(value));
      return ST_STOP;
    }

    return ST_CONTINUE;
  }
  
  if(sym_key == cl_vIds.default_)
  {
    if(TYPE(value) == T_SYMBOL)
    {
      ID sym = rb_to_id(value);
      if(sym != cl_vIds.none && cl_vIds.derive)
      {
        rb_raise(cl_eArgError, "Clips::Template#initialize :default key as symbol ony :none or :derive but '%s' was given.", CL_STR(value));
        return ST_STOP;
      }

      return ST_CONTINUE;
    }

    return ST_CONTINUE;
  }
  
  if(sym_key == cl_vIds.default_dynamic)
  {
    if(TYPE(value) != T_TRUE && TYPE(value) != T_FALSE)
    {
      rb_raise(cl_eArgError, "Clips::Template#initialize :default_dynamic key request true or false as value but '%s' have class '%s'.", CL_STR(value), CL_STR_CLASS(value));
      return ST_STOP;
    }

    return ST_CONTINUE;
  }

  if(sym_key == cl_vIds.constraint)
  {
    if(TYPE(value) != T_HASH && rb_obj_class(value) != cl_cConstraint)
    {
      rb_raise(cl_eArgError, "Clips::Template#initialize :constraint key request Constraint object or hash but '%s' have class '%s'.", CL_STR(value), CL_STR_CLASS(value));
      return ST_STOP;
    }

    return ST_CONTINUE;
  }
  
  rb_raise(cl_eArgError, "Clips::Template#initialize unknown slot configuration hash key '%s'", CL_STR(key));
  return ST_STOP;
}

/**
 * Return string reprezentation of self (CLIPS code)
 */
VALUE cl_template_to_s(VALUE self)
{
  return Qnil;
}
