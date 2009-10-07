#include <ruby.h>
#include "rbclips.h"
#include "rbgeneric.h"

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
