#include <ruby.h>
#include "clips/clips.h"
#include "rbclips.h"
#include "rbgeneric.h"
#include "rbfact.h"

/**
 * Transfer ruby ID to string representation that is valid in CLIPS.
 * :symbol => SYMBOL, :any => ?VARIABLE 
 */
const char const * rb_generic_clipstype_str(ID id)
{
# define TRANSFER(source, target) if(id == cl_vIds.source) return target
  TRANSFER(symbol,            "SYMBOL");
  TRANSFER(string,            "STRING");
  TRANSFER(lexeme,            "LEXEME");
  TRANSFER(integer,           "INTEGER");
  TRANSFER(float_,            "FLOAT");
  TRANSFER(number,            "NUMBER");
  TRANSFER(instance_name,     "INSTANCE-NAME");
  TRANSFER(instance_address,  "INSTANCE-ADDRESS");
  TRANSFER(instance,          "INSTANCE");
  TRANSFER(external_address,  "EXTERNAL-ADDRESS");
  TRANSFER(fact_address,      "FACT-ADDRESS");
  TRANSFER(any,               "?VARIABLE");
# undef TRANSFER

  return "";
}


/**
 * Convert given variable into string and do conversion if needed
 */
const char const * rb_generic_slot_value(VALUE value)
{
  if(TYPE(value) == T_SYMBOL)
  {
    // Check for reserved values
    ID sym = rb_to_id(value);
    if(sym == cl_vIds.one) return "?";
    if(sym == cl_vIds.all) return "$?";

    // Conversion to variable
    VALUE pom = rb_str_new_cstr("?");
    rb_str_catf(pom, "%s", CL_STR(value));

    return CL_STR(pom);
  }

  // Default case
  return CL_STR_ESCAPE(value);
}

/**
 * Check if given value is valid CLIPS symbol (don't have spaces)
 */
int cl_generic_check_clips_symbol(VALUE s)
{
  VALUE argv[1];
  argv[0] = rb_str_new2("^[^ \"]+$");

  VALUE regexp = rb_class_new_instance(1, argv, rb_cRegexp);

  VALUE ret = rb_funcall(regexp, cl_vIds.eqq, 1, s);

  if(TYPE(ret) == T_TRUE) return true;
  
  return false;
}

/**
 * Convert value on index from multifield DATA_OBJECT to corresponding
 * object in ruby world and return it
 */
VALUE cl_generic_convert_dataobject_mf(void *mf, int index)
{
  void *value = GetMFValue(mf, index);

  switch(GetMFType(mf, index))
  {
    case INTEGER:
      return INT2NUM( ValueToInteger(value) );

    case SYMBOL:
    {
      char *s = ValueToString(value);
      if(strcmp(s, "nil") == 0 ) return Qnil;

      return ID2SYM( rb_intern(s) );
    }
    case STRING:
      return rb_str_new_cstr( ValueToString(value) );

    case FLOAT:
      return rb_float_new( ValueToDouble(value) );

    default:
      break;
  }

  return Qfalse;
}

/**
 * Convert supplied DATA_OBJECT to 
 */
VALUE cl_generic_convert_dataobject(DATA_OBJECT value)
{
  switch(GetType(value))
  {
    case INTEGER:
      return INT2NUM( DOToInteger(value) );

    case SYMBOL:
    {
      char *s = DOToString(value);
      if(strcmp(s, "nil") == 0 ) return Qnil;

      return ID2SYM( rb_intern(s) );
    }

    case STRING:
      return rb_str_new_cstr( DOToString(value) );

    case FLOAT:
      return rb_float_new( DOToDouble(value) );

    case MULTIFIELD:
    {
      VALUE ret = rb_ary_new();

      int i;
      void *mf = GetValue(value);
      for(i = GetDOBegin(value); i <= GetDOEnd(value); i++)
      {
        VALUE slot = cl_generic_convert_dataobject_mf( mf, i );
        rb_ary_push(ret, slot);
      }

      return ret;
    }

    case FACT_ADDRESS:
    {
      // Fact addres
      void *fact = DOToPointer(value);

      // Creating the object
      cl_sFactWrap *wrap = calloc(1, sizeof(*wrap));
      VALUE ret = Data_Wrap_Struct(cl_cFact, NULL, free, wrap);
  
      // Building it's content
      wrap->ptr = fact;
      CL_UPDATE(ret);
      return ret;
    }

    default:
      break;
  }

  return Qfalse;
}

