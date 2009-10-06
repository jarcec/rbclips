#include <stdlib.h>
#include <ruby.h>
#include "clips/clips.h"
#include "rbclips.h"
#include "rbtemplate.h"
#include "rbconstraint.h"
#include "rbenvironment.h"
#include "rbexception.h"
#include "rbbase.h"

/* Definitions */
VALUE cl_cTemplate;
VALUE cl_cTemplateCreator;

//! Creating new Constraint object using block
VALUE cl_template_initialize_block(VALUE, VALUE);

//! Creating new Constraint object using hash
VALUE cl_template_initialize_hash(VALUE, VALUE);

//! Go throw configuring hash with slots
int cl_template_initialize_hash_each(VALUE, VALUE, VALUE);

//! Set up given variable (if it's valid)
int cl_template_initialize_check_variable(VALUE, VALUE, VALUE);

//! One by one process inner slot and save them
int cl_template_to_s_slot(VALUE, VALUE, VALUE);

//! Check template name
int cl_template_check_clips_symbol(VALUE);

/**
 * Creating new object - wrap struct
 */
VALUE cl_template_new(int argc, VALUE *argv, VALUE self)
{
  cl_sTemplateWrap *wrap = calloc(1, sizeof(*wrap));

  VALUE ret = Data_Wrap_Struct(self, NULL, free, wrap);
  rb_obj_call_init(ret, argc, argv);

  return ret;
}

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

  if( !cl_template_check_clips_symbol(name) )
  {
    rb_raise(cl_eArgError, "Clips::Template#intialize Name '%s' is not valid CLIPS template name .", CL_STR(name));
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

      // Check it not exists
      if( !NIL_P(rb_hash_lookup(s, entry)))
      {
        rb_raise(cl_eArgError, "Clips::Template#initialize redefinition of slot '%s'", CL_STR(entry) );
        return Qnil;
      }

      rb_hash_aset(s, entry, rb_hash_new());
    }
  }

  if(TYPE(slots) == T_HASH)
  {
    rb_hash_foreach(slots, cl_template_initialize_hash_each, s);
  }

  // At least one slot have to be defined
  if(NUM2LONG(rb_funcall(s, cl_vIds.size, 0)) == 0)
  {
    rb_raise(cl_eUseError, "Clips::Template must have at least one slot!");
    return Qnil;
  }

  // Saving to class
  rb_iv_set(self, "@name", name);
  rb_iv_set(self, "@slots", s);

  return Qtrue;
}

/**
 * Build template from configuring block
 */
VALUE cl_template_initialize_block(VALUE self, VALUE name)
{
  cl_sTemplateCreatorWrap *wrap = calloc(1, sizeof(*wrap));
  wrap->ptr = self;

  VALUE creator = Data_Wrap_Struct(cl_cTemplateCreator, NULL, free, wrap);
  VALUE s = rb_hash_new();

  if( !cl_template_check_clips_symbol(name) )
  {
    rb_raise(cl_eArgError, "Clips::Template#intialize Name '%s' is not valid CLIPS template name .", CL_STR(name));
    return Qnil;
  }

  rb_iv_set(self, "@name", name);
  rb_iv_set(self, "@slots", s);

  rb_yield(creator);

  // At least one slot have to be defined
  if(NUM2LONG(rb_funcall(s, cl_vIds.size, 0)) == 0)
    rb_raise(cl_eUseError, "Clips::Template must have at least one slot!");

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

  // Check it not exists - never executed :-S TODO
  if( !NIL_P(rb_hash_lookup(s, key)))
  {
    rb_raise(cl_eArgError, "Clips::Template#initialize redefinition of slot '%s'", CL_STR(key) );
    return Qnil;
  }

  // Check if it's valid slot configuration
  VALUE target = rb_hash_new();
  rb_hash_foreach(value, cl_template_initialize_check_variable, target);

  // Save this slot, it's valid
  rb_hash_aset(s, key, target);

  // C'est tout
  return ST_CONTINUE;
}

/**
 * Second line of configuring hash, parse and check options for one particular slot
 */
int cl_template_initialize_check_variable(VALUE key, VALUE value, VALUE target)
{
  // Check argument type
  if(TYPE(key) != T_SYMBOL && TYPE(key) != T_STRING)
  {
    rb_raise(cl_eArgError, "Key '%s' have class '%s' but expected was Symbol or String.", CL_STR(key), CL_STR_CLASS(value) );
    return ST_STOP;
  }

  // Changing key to symbol
  key = rb_funcall(key, cl_vIds.to_sym, 0);

  // Transfer it to ID
  ID sym_key = rb_to_id(key);

  if(sym_key == cl_vIds.multislot)
  {
    if(TYPE(value) != T_TRUE && TYPE(value) != T_FALSE)
    {
      rb_raise(cl_eArgError, "Clips::Template#initialize :multislot key request true or false as value but '%s' have class '%s'.", CL_STR(value), CL_STR_CLASS(value));
      return ST_STOP;
    }

    rb_hash_aset(target, key, value);
    return ST_CONTINUE;
  }
  
  if(sym_key == cl_vIds.default_)
  {
    rb_hash_aset(target, key, value);
    return ST_CONTINUE;
  }
  
  if(sym_key == cl_vIds.default_dynamic)
  {
    if(TYPE(value) != T_TRUE && TYPE(value) != T_FALSE)
    {
      rb_raise(cl_eArgError, "Clips::Template#initialize :default_dynamic key request true or false as value but '%s' have class '%s'.", CL_STR(value), CL_STR_CLASS(value));
      return ST_STOP;
    }

    rb_hash_aset(target, key, value);
    return ST_CONTINUE;
  }

  if(sym_key == cl_vIds.constraint)
  {
    if(TYPE(value) != T_HASH && rb_obj_class(value) != cl_cConstraint)
    {
      rb_raise(cl_eArgError, "Clips::Template#initialize :constraint key request Constraint object or hash but '%s' have class '%s'.", CL_STR(value), CL_STR_CLASS(value));
      return ST_STOP;
    }

    // Creating Object if hash is passed in place
    if( TYPE(value) == T_HASH )
      value = rb_class_new_instance(1, &value, cl_cConstraint);

    rb_hash_aset(target, key, value);
    return ST_CONTINUE;
  }
  
  rb_raise(cl_eArgError, "Clips::Template#initialize unknown slot configuration hash key '%s'", CL_STR(key));
  return ST_STOP;
}

/**
 * Go throw slots and convert (create) the appropriate text columns
 */
int cl_template_to_s_slot(VALUE key, VALUE value, VALUE target)
{
  // Possible values
  VALUE multislot  = rb_hash_lookup(value, ID2SYM(cl_vIds.multislot) );
  VALUE default_  = rb_hash_lookup(value, ID2SYM(cl_vIds.default_) );
  VALUE default_dynamic  = rb_hash_lookup(value, ID2SYM(cl_vIds.default_dynamic) );
  VALUE constraint  = rb_hash_lookup(value, ID2SYM(cl_vIds.constraint) );

  // Slot definition
  if(TYPE(multislot) == T_TRUE)
    rb_str_catf(target, " (multislot %s", CL_STR(key));
  else
    rb_str_catf(target, " (slot %s", CL_STR(key));

  // Default
  if( ! NIL_P(default_) )
  {
    // Default or default-dynamic?
    if(TYPE(default_dynamic) == T_TRUE)
      rb_str_cat2(target, " (default-dynamic ");
    else
      rb_str_cat2(target, " (default ");

    // Special keyword ?DERIVE/?NONE or some string/object?
    if( TYPE(default_) == T_SYMBOL)
    {
      ID s = rb_to_id(default_);
      if      (s == cl_vIds.derive)   rb_str_cat2(target, "?DERIVE)");
      else if (s == cl_vIds.none)     rb_str_cat2(target, "?NONE)");
      else                            rb_str_catf(target, "%s)", CL_STR(default_));

    } else if(TYPE(default_) == T_STRING)
        rb_str_catf(target, "\"%s\")", CL_STR(default_));
      else
        rb_str_catf(target, "%s)", CL_STR(default_));
  }

  // Constraint
  if( ! NIL_P(constraint) )
  {
    rb_str_catf(target, " %s", CL_STR(constraint));
  }

  // C'est tout
  rb_str_cat2(target, ")");
  return ST_CONTINUE;
}

/**
 * Return string reprezentation of self (CLIPS code)
 */
VALUE cl_template_to_s(VALUE self)
{
  VALUE ret = rb_str_new2("(deftemplate ");

  // Name
  VALUE name = rb_iv_get(self, "@name");
  rb_str_catf(ret, "%s", STR2CSTR(name) );

  // Slots
  VALUE slots = rb_iv_get(self, "@slots");
  rb_hash_foreach(slots, cl_template_to_s_slot, ret);

  // End
  rb_str_cat2(ret, " )");

  // C'est tout
  return ret;
}

/**
 * Make a copy of this object
 */
VALUE cl_template_clone(VALUE self)
{
  cl_sTemplateWrap *selfwrap = DATA_PTR(self);
  cl_sTemplateWrap *wrap = calloc( 1, sizeof(*wrap) );
  
  wrap->ptr = selfwrap->ptr;

  VALUE ret = Data_Wrap_Struct(cl_cTemplate, NULL, free, wrap);

  VALUE argv[1];
  argv[0] = rb_hash_new();
  rb_hash_aset(argv[0], ID2SYM(cl_vIds.name), rb_iv_get(self, "@name"));
  rb_hash_aset(argv[0], ID2SYM(cl_vIds.slots), rb_iv_get(self, "@slots"));

  rb_obj_call_init(ret, 1, argv);

  return ret;
}

/**
 * Represent this two objects same template?
 */
VALUE cl_template_equal(VALUE a, VALUE b)
{
  CL_EQUAL_DEFINE;
  
  CL_EQUAL_CHECK_IV("@name");
  CL_EQUAL_CHECK_IV("@slots");
  
  CL_EQUAL_DEFINE_WRAP(cl_sTemplateWrap);
  CL_EQUAL_CHECK_PTR;

  return Qtrue;
}

/**
 * Save this template to CLIPS environment
 */
VALUE cl_template_save(VALUE self)
{
  // This return false if exception was raised
  VALUE ret = cl_base_insert_command(Qnil, CL_TO_S(self));

  if(TYPE(ret) != T_FALSE)
  {
    cl_sTemplateWrap *wrap = DATA_PTR(self);
    wrap->ptr = FindDeftemplate( CL_STR(rb_iv_get(self, "@name" )) );
  }

  return Qtrue;
}

/**
 * Remove template from CLIPS if there is.
 */
VALUE cl_template_destroy(VALUE self)
{
  // May be dangerous - rather firstly update everything
  rb_funcall(self, cl_vIds.update, 0);

  cl_sTemplateWrap *wrap = DATA_PTR(self);

  if( !wrap || !wrap->ptr )
  {
      rb_raise(cl_eUseError, "Cannot destroy not saved template.");
      return Qnil;
  }

  // Return
  VALUE ret = Qfalse;

  if( Undeftemplate(wrap->ptr) )
    ret = Qtrue;

  // It doesn't matter how the undeftemplate ends - the template is not in CLIPS anyway...
  wrap->ptr = NULL;

  return ret;
}

/**
 * Template objects keeps inside pointers to CLIPS structures and that's problematic -
 * The pointer don't have to be valid, update function update the ruby object as is saved
 * in CLIPS.
 */
VALUE cl_template_update(VALUE self)
{
  // Self wrap
  cl_sTemplateWrap *wrap = DATA_PTR(self);
  VALUE name = rb_iv_get(self, "@name");

  wrap->ptr = FindDeftemplate( STR2CSTR(name) );

  return Qtrue;
}

/**
 * Constructor checks if this object was created by Clips::Template. If not it raise an
 * exception, bacause it's prohibited.
 */
VALUE cl_template_creator_initialize(VALUE self)
{
  cl_sTemplateCreatorWrap *wrap = DATA_PTR(self);
  if( !wrap || !wrap->ptr )
  {
      rb_raise(cl_eUseError, "Direct creation of Clips::Template::Creator is prohibited.");
      return Qnil;
  }

  return Qtrue;
}

/**
 * Set given slot for an object
 */
VALUE cl_template_creator_slot(int argc, VALUE *argv, VALUE self)
{
  // Wrapping pointer
  cl_sTemplateCreatorWrap *wrap = DATA_PTR(self);
  if( !wrap || !wrap->ptr )
  {
      rb_raise(cl_eUseError, "Direct use of Clips::Template::Creator is prohibited.");
      return Qnil;
  }

  // The slots hash
  VALUE s = rb_iv_get(wrap->ptr, "@slots");

  // Params check
  if(argc == 1 || argc == 2)
  {
    if(TYPE(argv[0]) != T_SYMBOL && TYPE(argv[0]) != T_STRING)
    {
      rb_raise(cl_eArgError, "Clips::Template::Creator#slot first argument have to be string or symbols but '%s' have class '%s'.", CL_STR(argv[0]), CL_STR_CLASS(argv[0]));
      return Qfalse;
    }

    if(argc == 2 && TYPE(argv[1]) != T_HASH)
    {
      rb_raise(cl_eArgError, "Clips::Template::Creator#slot second argument have to be hash but '%s' have class '%s'.", CL_STR(argv[1]), CL_STR_CLASS(argv[1]));
      return Qfalse;
    }
  } else {
    rb_raise(cl_eArgError, "Clips::Template::Creator#slot needs one or two arguments.");
    return Qnil;
  }

  // Check it not exists
  if( !NIL_P(rb_hash_lookup(s, argv[0])))
  {
    rb_raise(cl_eArgError, "Clips::Template#initialize redefinition of slot '%s'", CL_STR(argv[0]) );
    return Qnil;
  }

  // Inserted value
  VALUE value = rb_hash_new();

  // If we have second argument, process it
  if(argc == 2)
  {
    rb_hash_foreach(argv[1], cl_template_initialize_check_variable, value);
  }

  // Transfer type if necessary
  if(TYPE(argv[0]) == T_STRING)
    argv[0] =  rb_funcall(argv[0], cl_vIds.to_sym, 0);

  // Save this slot, it's valid
  rb_hash_aset(s, argv[0], value);

  // C'est tout
  return Qtrue;
}

/**
 * Check if given value is valid CLIPS symbol (don't have spaces)
 */
int cl_template_check_clips_symbol(VALUE s)
{
  VALUE argv[1];
  argv[0] = rb_str_new2("^[^ \"]+$");

  VALUE regexp = rb_class_new_instance(1, argv, rb_cRegexp);

  VALUE ret = rb_funcall(regexp, cl_vIds.eqq, 1, s);

  if(TYPE(ret) == T_TRUE) return true;
  
  return false;
}
