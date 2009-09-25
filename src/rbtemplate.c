#include <stdlib.h>
#include <ruby.h>
#include "clips/clips.h"
#include "rbclips.h"
#include "rbtemplate.h"
#include "rbenvironment.h"
#include "rbexception.h"

/* Definitions */
VALUE cl_cTemplate;

//! Creating new Constraint object using block
VALUE cl_template_initialize_block(VALUE, VALUE);

//! Creating new Constraint object using hash
VALUE cl_template_initialize_hash(VALUE, VALUE);

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
  return Qnil;
}

/**
 * Build template from block configuring block
 */
VALUE cl_template_initialize_block(VALUE self, VALUE name)
{
  return Qnil;
}

/**
 * Return string reprezentation of self (CLIPS code)
 */
VALUE cl_template_to_s(VALUE self)
{
  return Qnil;
}
