#include <stdlib.h>
#include <ruby.h>
#include "clips/clips.h"
#include "rbclips.h"
#include "rbtemplate.h"
#include "rbgeneric.h"
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
 * Load template with given name from CLIPS and create it's 
 * representation in ruby
 */
VALUE cl_template_load(VALUE self, VALUE name)
{
  if(TYPE(name) != T_STRING && TYPE(name) != T_SYMBOL)
  {
    rb_raise(cl_eArgError, "Clips::Template::load expect argument of type symbol or string, but '%s' have class '%s'", CL_STR(name), CL_STR_CLASS(name));
    return Qnil;
  }

  // Looking for the template
  void *template = FindDeftemplate( CL_STR(name) );
  if(!template) return Qnil;

  // Creating the object
  cl_sTemplateWrap *wrap = calloc(1, sizeof(*wrap));
  VALUE ret = Data_Wrap_Struct(cl_cTemplate, NULL, free, wrap);
  
  // Building it's content
  rb_iv_set(ret, "@name", CL_TO_S(name));
  CL_UPDATE(ret);

  // C'est tout
  return ret;
}

/**
 * Return array with all templates that are saved in CLIPS
 */
VALUE cl_template_all(VALUE self)
{
  VALUE ret = rb_ary_new();

  void *template = NULL;

  while( template = GetNextDeftemplate(template) )
  {
    // Skip implied templates (it's not real template)
    if( ((struct deftemplate*)template)->implied ) continue;
    
    // I'm not interested in default/initial deftemplate
    char *name = GetDeftemplateName(template);
    if(strcmp(name, "initial-fact") == 0) continue;

    // Creating the object
    cl_sTemplateWrap *wrap = calloc(1, sizeof(*wrap));
    VALUE obj = Data_Wrap_Struct(cl_cTemplate, NULL, free, wrap);
  
    // Building it's content
    rb_iv_set(obj, "@name", rb_str_new_cstr(name));
    CL_UPDATE(obj);

    rb_ary_push(ret, obj);
  }

  // C'est tout
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

  if( !cl_generic_check_clips_symbol(name) )
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

      if( !cl_generic_check_clips_symbol(entry) )
      {
        rb_raise(cl_eArgError, "Clips::Template#intialize Name '%s' is not valid CLIPS template name .", CL_STR(entry));
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

  if( !cl_generic_check_clips_symbol(name) )
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
  if( !cl_generic_check_clips_symbol(key) )
  {
    rb_raise(cl_eArgError, "Clips::Template#intialize Name '%s' is not valid CLIPS template name .", CL_STR(key));
    return Qnil;
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

    } else
        rb_str_catf(target, "%s)", CL_STR_ESCAPE(default_));
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
  CL_UPDATE(self);

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
  CL_UPDATE(a);
  CL_UPDATE(b);

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
 * Is the template saved in CLIPS?
 */
VALUE cl_template_saved(VALUE self)
{
  CL_UPDATE(self);
  cl_sTemplateWrap *wrap = DATA_PTR(self);

  if( !wrap || !wrap->ptr ) return Qfalse;

  return Qtrue;
}

/**
 * Remove template from CLIPS if there is.
 */
VALUE cl_template_destroy(VALUE self)
{
  CL_UPDATE(self);

  cl_sTemplateWrap *wrap = DATA_PTR(self);

  if( !wrap )
  {
    rb_raise(cl_eUseError, "Inner structure not found");
    return Qnil;
  }

  if( !wrap->ptr) return Qfalse;
 
  // Template cannot be in use when deleting
  if( !IsDeftemplateDeletable(wrap->ptr) )
  {
    rb_raise(cl_eInUseError, "Template is used, cannot be deleted");
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
 *
 * This code will so far loose information about constraints on given slot
 * TODO: Add creation of constraint objects for slots!
 */
VALUE cl_template_update(VALUE self)
{
  // Self wrap
  cl_sTemplateWrap *wrap = DATA_PTR(self);
  VALUE name = rb_iv_get(self, "@name");

  wrap->ptr = FindDeftemplate( STR2CSTR(name) );

  // The deftemplate don't have to be saved
  if( !wrap->ptr ) return Qfalse;

  // New slots names
  VALUE slots = rb_hash_new();

  // Getting slot list from CLIPS
  DATA_OBJECT slotNames;
  DeftemplateSlotNames(wrap->ptr, &slotNames);
  if(GetType(slotNames) != MULTIFIELD)
  {
    rb_raise(cl_eInternError, "slotNames should be a multifield value and it's not.");
    return Qnil;
  }

  // Transcoding CLIPS slotnames to ruby names and saving them into slot list
  int i;
  void *mf = GetValue(slotNames);
  for(i = GetDOBegin(slotNames); i <= GetDOEnd(slotNames); i++)
  {
    VALUE slot = cl_generic_convert_dataobject_mf( mf, i );
    rb_hash_aset(slots, slot, rb_hash_new());
  }

  // Saving new slot list and c'es tout
  rb_iv_set(self, "@slots", slots);
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

    if( !cl_generic_check_clips_symbol(argv[0]) )
    {
      rb_raise(cl_eArgError, "Clips::Template#intialize Name '%s' is not valid CLIPS template name .", CL_STR(argv[0]));
      return Qnil;
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

