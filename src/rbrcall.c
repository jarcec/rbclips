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
