/*
 * Copyright 2013 Jarek Jarcec Cecho <jarcec@apache.org>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdio.h>
#include <ruby.h>
#include "clips/clips.h"
#include "rbclips.h"
#include "rbexception.h"
#include "rbconstraint.h"
#include "rbgeneric.h"

/* Definitions */
VALUE cl_cConstraint;
VALUE cl_cConstraintCreator;

//! Creating new Constraint object using block
VALUE cl_constraint_initialize_block(VALUE);

//! Creating new Constraint object using hash
VALUE cl_constraint_initialize_hash(VALUE, VALUE);

//! Set up given variable (if it's valid)
int cl_constraint_initialize_setup_variable(VALUE, VALUE, VALUE);

//! Is it CLIPS type? (pseudovariable ClipsType in documentation)
int cl_constraint_check_clipstype(VALUE, int);

/**
 * Constructor for creating
 */
VALUE cl_constraint_initialize(int argc, VALUE *argv, VALUE self)
{
  if(argc == 0 && rb_block_given_p())
  {
    return cl_constraint_initialize_block(self);
  }

  if(argc == 1 && TYPE(argv[0]) == T_HASH)
  {
    return cl_constraint_initialize_hash(self, argv[0]);
  }

  rb_raise(cl_eArgError, "Clips::Constrait#initialize called with wrong arguments, required is one Hash argument or none argument with block.");
  return Qnil;
}

/**
 * Goes throw hash and check known keys if their valid and set up the 
 * contraint object (saving values to instance variables)
 */
VALUE cl_constraint_initialize_hash(VALUE self, VALUE hash)
{
  rb_hash_foreach(hash, cl_constraint_initialize_setup_variable, self);
  return Qtrue;
}

/**
 * Create object Clips::Constraint::Creator and pass it to given block,
 * where user can set up the constraint object
 */
VALUE cl_constraint_initialize_block(VALUE self)
{
  cl_sConstraintCreatorWrap *wrap = calloc(1, sizeof(*wrap));
  wrap->ptr = self;

  VALUE creator = Data_Wrap_Struct(cl_cConstraintCreator, NULL, free, wrap);

  rb_yield(creator);

  return Qnil;
}

/**
 * Check if key is valid and if so set up the variable.
 *  :type => value results in @type = value
 *    @type is always an array, so if value.class != Array then [value]
 *  :values => [value, value] in @values = [value, value]
 *  :range => Range  in  @range => Range
 *  :cardinality => Range in @cardinality => Range
 */
int cl_constraint_initialize_setup_variable(VALUE key, VALUE value, VALUE self)
{
  // Check argument type
  if(TYPE(key) != T_SYMBOL && TYPE(key) != T_STRING)
  {
    rb_raise(cl_eArgError, "Key '%s' have class '%s' but expected was Symbol or String.", CL_STR(key), CL_STR_CLASS(value) );
    return ST_STOP;
  }

  // Transfer it to ID
  ID sym_key = rb_to_id(key);

  // :type => value | [value, value, ...]
  if(sym_key == cl_vIds.type)
  {
    VALUE vv;

    switch(TYPE(value))
    {
      case T_SYMBOL:
      case T_STRING:
        if( !cl_constraint_check_clipstype(value, true) )
        {
          rb_raise(cl_eArgError, "'%s' is not a valid ClipsType", CL_STR(value));
          return ST_STOP;
        }
        vv = rb_ary_new();
        rb_ary_push(vv, value);
        break;
      case T_ARRAY:
          {
            long len = RARRAY_LEN(value);
            long i;
            for(i = 0; i < len; i++)
              if( !cl_constraint_check_clipstype( rb_ary_entry(value, i), false ) )
              {
                 rb_raise(cl_eArgError, "'%s' is not a valid ClipsType", CL_STR(value));
                 return ST_STOP;
              }
          }
          vv = value;
        break;
      default:
        rb_raise(cl_eArgError, "Hash key 'type' allow only symbor, string or array as an value.");
        return ST_STOP;
    }

    rb_iv_set(self, "@type", vv);
    return ST_CONTINUE;
  }

  // :values => [ 'ahoj', ahoj, 2, 5, ...]
  if(sym_key == cl_vIds.values)
  {
    if(TYPE(value) != T_ARRAY)
    {
      rb_raise(cl_eArgError, "Hash key 'values' key needs and array as value, but '%s' have class '%s'", CL_STR(value), CL_STR_CLASS(value));
      return ST_STOP;
    }

    rb_iv_set(self, "@values", value);
    return ST_CONTINUE;
  }

  // :range => 0..2
  if(sym_key == cl_vIds.range)
  {
    if(rb_obj_class(value) == rb_cRange)
    {
      rb_iv_set(self, "@range", value);
      return ST_CONTINUE;
    }

    rb_raise(cl_eArgError, "Hash key 'range' key needs and Range as value, but '%s' have class '%s'", CL_STR(value), CL_STR_CLASS(value));
    return ST_STOP;
  }

  // :cardinality => 22..5
  if(sym_key == cl_vIds.cardinality)
  {
    if(rb_obj_class(value) == rb_cRange)
    {
      rb_iv_set(self, "@cardinality", value);
      return ST_CONTINUE;
    }

    rb_raise(cl_eArgError, "Hash key 'cardinality' key needs and Range as value, but '%s' have class '%s'", CL_STR(value), CL_STR_CLASS(value));
    return ST_STOP;
  }

  rb_raise(cl_eArgError, "Unknown option '%s' of class '%s' in config hash.", CL_STR(key), CL_STR_CLASS(key));
  return ST_STOP;
}

/**
 * Return true if the variable is one of allowed clips types or :any
 */
int cl_constraint_check_clipstype(VALUE key, int include_any)
{
  // Check argument type
  if(TYPE(key) != T_SYMBOL && TYPE(key) != T_STRING)
  {
    rb_raise(cl_eArgError, "Not given symbol or string variable");
    return false;
  }
  
  ID sym_key = rb_to_id(key);

# define CHECKTYPE(name) if(sym_key == cl_vIds.name) return true
  CHECKTYPE(string);
  CHECKTYPE(symbol);
  CHECKTYPE(lexeme);
  CHECKTYPE(integer);
  CHECKTYPE(float_);
  CHECKTYPE(number);
  CHECKTYPE(instance_name);
  CHECKTYPE(instance_address);
  CHECKTYPE(instance);
  CHECKTYPE(external_address);
  CHECKTYPE(fact_address);

  if(include_any)
    CHECKTYPE(any);
# undef CHECKTYPE

  return false;
}

/**
 * Return CLIPS fragment of code representing the constraint object.
 * Unfortunatelly the code don't have to be valid - the validity is not check
 * in this class, becuase constraint are not saved to CLIPS, so their are
 * checked when used.
 */
VALUE cl_constraint_to_s(VALUE self)
{
  VALUE ret = rb_str_new2("");

  // @type
  VALUE v = rb_iv_get(self, "@type");
  if(!NIL_P(v))
  {
    rb_str_cat2(ret, "(type ");

    long len = RARRAY_LEN(v);
    int i;
    for(i = 0; i < len; i++)
      rb_str_catf(ret, "%s ", rb_generic_clipstype_str( rb_to_id( rb_ary_entry(v, i ) ) ) );

    rb_str_cat2(ret, ") ");
  }

  // @values
  v = rb_iv_get(self, "@values");
  if(!NIL_P(v))
  {
    rb_str_cat2(ret, "(values ");

    long len = RARRAY_LEN(v);
    int i;
    for(i = 0; i < len; i++)
    {
      VALUE entry = rb_ary_entry(v, i);
      if(TYPE(entry) == T_STRING)
      {
        rb_str_catf(ret, "\"%s\" ", STR2CSTR( entry ) );
      } else {
        rb_str_catf(ret, "%s ", CL_STR(entry) );
      }
    }

    rb_str_cat2(ret, ") ");
  }

  // @range
  v = rb_iv_get(self, "@range");
  if(!NIL_P(v))
  {
    VALUE begin = rb_funcall(v, cl_vIds.begin, 0);
    VALUE end   = rb_funcall(v, cl_vIds.end, 0);

    long lbegin = NUM2LONG(begin);
    long lend   = NUM2LONG(end);

    rb_str_catf(ret, "(range %lu %lu) ",  lbegin, lend );
  }

  // @cardinality
  v = rb_iv_get(self, "@cardinality");
  if(!NIL_P(v))
  {
    VALUE begin = rb_funcall(v, cl_vIds.begin, 0);
    VALUE end   = rb_funcall(v, cl_vIds.end, 0);

    long lbegin = NUM2LONG(begin);
    long lend   = NUM2LONG(end);

    rb_str_catf(ret, "(cardinality %lu %lu) ",  lbegin, lend );
  }

  // C'est tout
  return ret;
}

/**
 * Represent the two objects same constraint?
 */
VALUE cl_constraint_equal(VALUE a, VALUE b)
{
  CL_EQUAL_CLASS(b, cl_cConstraint);

  CL_EQUAL_DEFINE;
  
  CL_EQUAL_CHECK_IV("@type");
  CL_EQUAL_CHECK_IV("@values");
  CL_EQUAL_CHECK_IV("@cardinality");
  CL_EQUAL_CHECK_IV("@range");

  return Qtrue;
}

/**
 * Constructor only protect direct creating instance,
 * because it is useless otherwise. Clips::Constraint::Creator 
 * make sense only in use when creating Clips::Constraint object.
 */
VALUE cl_constraint_creator_initialize(VALUE self)
{
  cl_sConstraintCreatorWrap *wrap = DATA_PTR(self);
  if( !wrap || !wrap->ptr )
  {
      rb_raise(cl_eUseError, "Direct creation of Clips::Constraint::Creator is prohibited.");
      return Qnil;
  }

  return Qtrue;
}

/*
 * Template functions for Constraint::Creator
 */
# define CREATOR_INNER(what) \
      cl_sConstraintCreatorWrap *wrap = DATA_PTR(self); \
      if( !wrap || !wrap->ptr ) \
      { \
        rb_raise(cl_eUseError, "Direct use of Clips::Constraint::Creator is prohibited."); \
        return Qnil; \
      } \
      \
      cl_constraint_initialize_setup_variable(rb_str_new2(what), param, wrap->ptr); \
      \
      return Qtrue

VALUE cl_constraint_creator_type(VALUE self, VALUE param)
{
  CREATOR_INNER("type");
}
VALUE cl_constraint_creator_values(VALUE self, VALUE param)
{
  CREATOR_INNER("values");
}
VALUE cl_constraint_creator_range(VALUE self, VALUE param)
{
  CREATOR_INNER("range");
}
VALUE cl_constraint_creator_cardinality(VALUE self, VALUE param)
{
  CREATOR_INNER("cardinality");
}
# undef CREATOR_SET
