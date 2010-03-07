#include <stdio.h>
#include <ruby.h>
#include "clips/clips.h"
#include "rbclips.h"
#include "rbrcall.h"
#include "rbgeneric.h"

/**
 * Define rcall function in CLIPS which
 * will call aour cl_rcall function.
 */
void cl_rcall_define()
{
  DefineFunction2("rcall", 'v', PTIF cl_rcall, "cl_rcall", "2*");
}

/**
 * CLIPS function that is responsible for calling
 * ruby objects
 */
void cl_rcall()
{
  int argc = RtnArgCount();

  // Sanity check
  if(argc < 2) return;

  // Get object and method
  char *s_obj = RtnLexeme(1);
  char *s_mtd = RtnLexeme(2);

  // Get variant as I need them
  VALUE obj; sscanf(s_obj, "%lx", &obj);
  ID mtd = rb_intern(s_mtd);

  if(argc == 2)
  {
    rb_funcall(obj, mtd, 0);
  } else {
    VALUE argv[argc - 2];
    int i;
    for(i = 3; i <= argc; i++)
    {
      DATA_OBJECT argument;
      RtnUnknown(i, &argument);
      argv[i - 3] = cl_generic_convert_dataobject(argument);
    }

    rb_funcall2(obj, mtd, argc-2, argv);
  }
}
