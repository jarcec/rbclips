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
VALUE cl_cFactAddress;

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

//! Foreach method for string conversion
int cl_fact_to_s_nonordered_each(VALUE, VALUE, VALUE);

//! Foreach method for updating fact
int cl_fact_update_each(VALUE, VALUE, VALUE);

//! Define method for ordered fact
void cl_fact_define_instance_methods_ordered(VALUE, int);

//! Define method for nonordered fact
void cl_fact_define_instance_methods_nonordered(VALUE, int);

//! Foreach method for creating slot accessors (nonordered fact)
int cl_fact_define_instance_methods_attr_slot(VALUE, VALUE, VALUE);

//! Method body (block) for creating slot accessors (nonordered fact)
VALUE cl_fact_define_instance_methods_attr_slot_block(VALUE, VALUE, int, VALUE *);

//! Method body (block) for creationt set_slot accessors (nonordered fact)
VALUE cl_fact_define_instance_methods_attr_set_slot_block(VALUE, VALUE, int, VALUE *);

//! Foreach method for creation set_slot accessors(nonordered fact)
int cl_fact_define_instance_methods_attr_set_slot(VALUE, VALUE, VALUE);

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
 * Build and return back array with all facts
 */
VALUE cl_fact_all(VALUE self)
{
  VALUE ret = rb_ary_new();

  void *fact = NULL;

  while( fact = GetNextFact(fact) )
  {
    char *templatename = GetDeftemplateName( FactDeftemplate(fact) );
    if(strcmp(templatename, "initial-fact") == 0) continue;

    // Creating the object
    cl_sFactWrap *wrap = calloc(1, sizeof(*wrap));
    VALUE obj = Data_Wrap_Struct(cl_cFact, NULL, free, wrap);
  
    // Building it's content
    wrap->ptr = fact;
    CL_UPDATE(obj);
    cl_fact_define_instance_methods(obj, FACT_FAMILY_READ|FACT_FAMILY_WRITE);

    rb_ary_push(ret, obj);
  }

  // C'est tout
  return ret;
}

/**
 * Return only facts matching given criteria
 * This is really simple approach, some more sofisticated 
 * way is desierable for the future.
 */
VALUE cl_fact_find(int argc, VALUE *argv, VALUE self)
{
  if(argc != 1)
  {
    rb_raise(cl_eArgError, "Clips::Fact::find Wrong number of argumentse, please read manual for allowed values.");
    return Qnil;
  }

  if(TYPE(argv[0]) != T_STRING && TYPE(argv[0]) != T_SYMBOL && rb_obj_class(argv[0]) != cl_cTemplate)
  {
    rb_raise(cl_eArgError, "Clips::Fact::find expects for first argument to be string, symbol or Template object, but '%s' have class '%s'.", CL_STR(argv[0]), CL_STR_CLASS(argv[0]));
    return Qnil;
  }

  // I'm using fact that CLIPS are creating templates even for 
  // ordered facts, so task to find out if we're looking for this
  // fact based on template is really simple and straightforward
  // even for ordered facts.
  char *template;
  if(rb_obj_class(argv[0]) == cl_cTemplate)
  {
    template = CL_STR( rb_iv_get(argv[0], "@name")  );
  } else {
    template = CL_STR(argv[0]);
  }

  VALUE ret = rb_ary_new();
  void *fact = NULL;

  while( fact = GetNextFact(fact) )
  {
    // Are we looking for this?
    char *templatename = GetDeftemplateName( FactDeftemplate(fact) );
    if(strcmp(template, templatename) != 0) continue;

    // Creating the object
    cl_sFactWrap *wrap = calloc(1, sizeof(*wrap));
    VALUE obj = Data_Wrap_Struct(cl_cFact, NULL, free, wrap);
  
    // Building it's content
    wrap->ptr = fact;
    CL_UPDATE(obj);
    cl_fact_define_instance_methods(obj, FACT_FAMILY_READ|FACT_FAMILY_WRITE);

    rb_ary_push(ret, obj);
  }

  // C'est tout
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

  // Define instance methods
  cl_fact_define_instance_methods_ordered(self, FACT_FAMILY_READ|FACT_FAMILY_WRITE);

  return Qtrue;
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

  // Define instance methods
  cl_fact_define_instance_methods_nonordered(self, FACT_FAMILY_READ|FACT_FAMILY_WRITE);

  return Qtrue;
}

/**
 * Return slot list in case of ordered fact
 */
VALUE cl_fact_slots(VALUE self)
{
  CL_UPDATE(self);

  VALUE slots = rb_iv_get(self, "@slots");
  return rb_ary_dup(slots);
}

/**
 * Return value of given slot (nonordered fact)
 */
VALUE cl_fact_slot(VALUE self, VALUE slot)
{
  CL_UPDATE(self);

  VALUE template  = rb_iv_get(self, "@template");
  VALUE fslots    = rb_iv_get(self, "@slots");
  VALUE tslots    = rb_iv_get(template, "@slots");

  // Check if given slot really exists  
  VALUE c  = rb_hash_lookup(tslots, slot);
  if(NIL_P(c))
  {
    rb_raise(cl_eArgError, "Clips::Fact#slot Given slot '%s' don't exists", CL_STR(slot));
    return Qnil;
  }
  
  return rb_hash_lookup(fslots, slot);
}

/**
 * Method for changing slot value in nonordered fact
 */
VALUE cl_fact_set_slot(VALUE self, VALUE slot, VALUE newValue)
{
  CL_UPDATE(self);

  cl_sFactWrap *wrap = DATA_PTR(self);
  if( !wrap )
  {
      rb_raise(cl_eUseError, "Oops, wrap structure don't exists!");
      return Qnil;
  }

  VALUE template  = rb_iv_get(self, "@template");
  VALUE fslots    = rb_iv_get(self, "@slots");
  VALUE tslots    = rb_iv_get(template, "@slots");

  // Check if given slot really exists  
  VALUE c  = rb_hash_lookup(tslots, slot);
  if(NIL_P(c))
  {
    rb_raise(cl_eArgError, "Clips::Fact#set_slot Given slot '%s' don't exists", CL_STR(slot));
    return Qnil;
  }
  
  // Save new value inside Ruby object
  rb_hash_aset(fslots, slot, newValue);

  // If we're saved, update CLIPS structure as well
  if(wrap->ptr)
  {
    long long index = FactIndex(wrap->ptr);
    VALUE command = rb_sprintf("(modify %lld (%s %s))", index, CL_STR(slot), CL_STR_ESCAPE(newValue));
    cl_base_insert_command(Qnil, command);
  }

  return Qtrue;
}

/**
 * If it's ordered fact, returns name of fact, otherwise return template object.
 * Basically return @template.
 */
VALUE cl_fact_template(VALUE self)
{
  VALUE ret = rb_iv_get(self, "@template");
  return rb_funcall(ret, cl_vIds.clone, 0);
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
    rb_str_catf(ret, " %s", CL_STR_ESCAPE(rb_ary_entry(slots, i)) );

  rb_str_cat2(ret, ")");
  return ret;
}

/**
 * to_s: Nonordered variant
 */
VALUE cl_fact_to_s_nonordered(VALUE self)
{
  VALUE ret       = rb_str_new2("(");
  VALUE template  = rb_iv_get(self, "@template");
  VALUE slots     = rb_iv_get(self, "@slots");
  VALUE tname     = rb_iv_get(template, "@name");
  rb_str_cat2(ret, CL_STR(tname));

  rb_hash_foreach(slots, cl_fact_to_s_nonordered_each, ret);

  rb_str_cat2(ret, ")");
  return ret;
}

/**
 * to_s: For each slot in hash
 */
int cl_fact_to_s_nonordered_each(VALUE key, VALUE value, VALUE target)
{
  // Skip this slot, if it's value is nil
  if(NIL_P(value))
    return ST_CONTINUE;

  // If its array ~= multifield value, do it one by one
  if(TYPE(value) == T_ARRAY)
  {
    rb_str_catf(target, " (%s", CL_STR(key));

    long len = RARRAY_LEN(value);
    int i;
    for(i = 0; i < len; i++)
      rb_str_catf(target, " %s", CL_STR_ESCAPE(rb_ary_entry(value, i)) );

    rb_str_cat2(target, ")");
    return ST_CONTINUE;
  } 

  // Normal object, convert it to string
  rb_str_catf(target, " (%s %s)", CL_STR(key), CL_STR_ESCAPE(value));

  return ST_CONTINUE;
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
  CL_EQUAL_CLASS(b, cl_cFact);

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

  // Is this fact already saved or not?
  if( !wrap->ptr )
  { 
    // No it's not, so we're creating new on
    wrap->ptr = AssertString( CL_STR(self) );
  } else {
    // This fact is already saved, so we're making same approach
    // as CLIPS itself - retracting current and asssert new one.
    // Modify command is not suitable here, because it "forget"
    // pointer to newly created fact.
    if( !Retract(wrap->ptr) ) return Qfalse;
    wrap->ptr = AssertString( CL_STR(self) );
  }

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
  {
    wrap->ptr = NULL;
    return Qtrue;
  }

  // Load template name
  void *template = FactDeftemplate(wrap->ptr);
  if( ((struct deftemplate*)template)->implied )
  {
    // Ordered fact
    rb_iv_set(self, "@template", rb_str_new_cstr(GetDeftemplateName(template)) );

    DATA_OBJECT slot;
    if( !GetFactSlot(wrap->ptr, NULL, &slot) )
    {
      rb_raise(cl_eInternError, "Cannot get implied slot of ordered fact, wtf?");
      return Qnil;
    }
    rb_iv_set(self, "@slots", cl_generic_convert_dataobject(slot) );

  } else {
    // Non ordered
    VALUE template_obj = rb_funcall(cl_cTemplate, cl_vIds.load, 1, rb_str_new_cstr(GetDeftemplateName(template)));
    rb_iv_set(self, "@template", template_obj);
    
    VALUE value_slots = rb_hash_new();
    rb_iv_set(self, "@slots", value_slots);

    VALUE key_slots = rb_iv_get(template_obj, "@slots");
    rb_hash_foreach(key_slots, cl_fact_update_each, self);
  }

  return Qtrue;
}

/**
 * Foreach function that goes throw all slots and load it's values from
 * CLIPS and store them inside object
 */
int cl_fact_update_each(VALUE key, VALUE value, VALUE self)
{
  cl_sFactWrap *wrap = DATA_PTR(self);
  if( !wrap )
  {
      rb_raise(cl_eUseError, "Oops, wrap structure don't exists!");
      return ST_STOP;
  }

  DATA_OBJECT slot;
  if( !GetFactSlot(wrap->ptr, CL_STR(key) , &slot) )
  {
    rb_raise(cl_eInternError, "Cannot get implied slot of ordered fact, wtf?");
    return ST_STOP;
  }

  VALUE slots = rb_iv_get(self, "@slots");
  rb_hash_aset(slots, key, cl_generic_convert_dataobject(slot));

  return ST_CONTINUE;
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

/**
 * Define instance methods based on self's type 
 * (ordered or nonordered fact).
 */
void cl_fact_define_instance_methods(VALUE self, int family)
{
  VALUE template = rb_iv_get(self, "@template");

  if(TYPE(template) == T_STRING) cl_fact_define_instance_methods_ordered(self, family);

  if(rb_obj_class(template) == cl_cTemplate) cl_fact_define_instance_methods_nonordered(self, family);
}

/**
 * Define instance methods in case of ordered fact
 */
void cl_fact_define_instance_methods_ordered(VALUE self, int family)
{
  if(family & FACT_FAMILY_READ)
  {
    // Define singleton methods
    rb_define_singleton_method(self, "slots", cl_fact_slots, 0);
    rb_define_singleton_method(self, "name", cl_fact_template, 0);
  }
}

/**
 * Define instance methods in case of nonordered fact
 */
void cl_fact_define_instance_methods_nonordered(VALUE self, int family)
{
  if(family & FACT_FAMILY_READ)
  {
    // Define singleton methods
    rb_define_singleton_method(self, "slot", cl_fact_slot, 1);
    rb_define_singleton_method(self, "template", cl_fact_template, 0);

    // Define slot accessors
    VALUE template = rb_iv_get(self, "@template");
    VALUE slots = rb_iv_get(template, "@slots");
    rb_hash_foreach(slots, cl_fact_define_instance_methods_attr_slot, self);
  }
  if(family & FACT_FAMILY_WRITE)
  {
    // Define set_slot accessors
    VALUE template = rb_iv_get(self, "@template");
    VALUE slots = rb_iv_get(template, "@slots");

    rb_define_singleton_method(self, "set_slot", cl_fact_set_slot, 2);
    rb_hash_foreach(slots, cl_fact_define_instance_methods_attr_set_slot, self);
  }
}

/**
 * Block represenatation for wrapping set_slot(name,newValue) method (body)
 */
VALUE cl_fact_define_instance_methods_attr_set_slot_block(VALUE yield, VALUE ary, int argc, VALUE *argv)
{
  if(argc != 1)
  {
    rb_raise(cl_eArgError, "Clips::Fact#set_slot slot '%s' called without any new value.", CL_STR(rb_ary_entry(ary, 1)));
    return Qnil;
  }

  return rb_funcall(rb_ary_entry(ary, 0), rb_intern("set_slot"), 2, rb_ary_entry(ary, 1), argv[0]);
}

/**
 * For each method for creating set_slot methods
 */
int cl_fact_define_instance_methods_attr_set_slot(VALUE key, VALUE value, VALUE self)
{
  VALUE ary = rb_ary_new();
  rb_ary_push(ary, self);
  rb_ary_push(ary, key);

  VALUE name = rb_sprintf("%s=", CL_STR(key));

  rb_block_call(self, rb_intern("define_singleton_method"), 1, &name, cl_fact_define_instance_methods_attr_set_slot_block, ary);

  return ST_CONTINUE;
}

/**
 * Block represenatation for wrapping slot(name) method (body)
 */
VALUE cl_fact_define_instance_methods_attr_slot_block(VALUE yield, VALUE ary, int argc, VALUE *argv)
{
  if(argc != 0)
  {
    rb_raise(cl_eArgError, "Clips::Fact#slot slot '%s' called with additional parameters.", CL_STR(rb_ary_entry(ary, 1)));
    return Qnil;
  }

  return rb_funcall(rb_ary_entry(ary, 0), rb_intern("slot"), 1, rb_ary_entry(ary, 1));
}

/**
 * For each method for creating slot attributes
 */
int cl_fact_define_instance_methods_attr_slot(VALUE key, VALUE value, VALUE self)
{
  VALUE ary = rb_ary_new();
  rb_ary_push(ary, self);
  rb_ary_push(ary, key);

  rb_block_call(self, rb_intern("define_singleton_method"), 1, &key, cl_fact_define_instance_methods_attr_slot_block, ary);

  return ST_CONTINUE;
}

/**
 * Return representation of our address
 */
VALUE cl_factaddress_to_s(VALUE self)
{
  return rb_str_dup( rb_iv_get(self, "@to_s") );
}
