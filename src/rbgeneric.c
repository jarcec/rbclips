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
