#include <ruby.h>
#include "rbclips.h"
#include "rbbase.h"
#include "rbrouter.h"
#include "rbenvironment.h"
#include "rbexception.h"
#include "clips/clips.h"

/* Definitions */
VALUE cl_cBase;

/**
 * Run given CLIPS command and return it's result
 */
VALUE cl_base_insert_command(VALUE self, VALUE cmd)
{
  // Param check
  if(TYPE(cmd) != T_STRING)
  {
    rb_raise(cl_eArgError, "Base::insert_command accept only String as parametr!");
    return Qfalse;
  }

  // Execution needs string ended by new line character and user may not
  // add it. It will work with two new line characters anyway, so I simply
  // append \n and I don't check whether the \n is alrady at the end
  VALUE c = rb_str_cat(cmd, "\n", 1);

  void *env = GetCurrentEnvironment();

  FlushCommandString(env);
  AppendCommandString(env, StringValueCStr(c));

  // Executing command
  if( ! ExecuteIfCommandComplete(env) || cl_router_werror())
  {
    char *msg = STR2CSTR(cl_router_get_content_d());

    if(strlen(msg) == 0)
    {
      rb_raise(cl_eException, "Unknown error, probably the inserted command is not valid CLIPS command.");
    } else {
      rb_raise(cl_eException, "%s",  msg );
    }

    return Qfalse;
  }

  // Returning output (if any)
  return cl_router_get_content_d();
}

/**
 * Start firing rules
 */
VALUE cl_base_run(int argc, VALUE *argv, VALUE self)
{
  if( !(argc == 0 || (argc == 1 && (TYPE(argv[0]) == T_FIXNUM || TYPE(argv[0]) == T_BIGNUM))) )
  {
    rb_raise(cl_eArgError, "Base::run accept only Fixnum or Bignum as parametr!");
    return Qnil;
  }

  long long count = -1;
  if(argc == 1)
    count = NUM2LONG(argv[0]);

  count = Run(count);

  return INT2NUM(count);
}

/**
 * Call CLIPS function of same name
 */
VALUE cl_base_reset(VALUE self)
{
  Reset();
  return Qnil;
}

