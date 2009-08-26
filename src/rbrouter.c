#include "rbclips.h"
#include "rbrouter.h"
#include "clips/clips.h"

/* Definitions */

//! CLIPS router query function
int cl_router_query(char *);

//! Output forwarding
int cl_router_print(char *, char *);

//! Input forwarding
int cl_router_getc(char *);

//! Return character back
int cl_router_ungetc(int, char *);

//! Shutdown routering system
int cl_router_exit(int);

//! String with output
VALUE cl_router_content;

/**
 * Set up all that's need to forward clips input/output into
 * local variables, that can be than transfer to ruby world.
 */
void cl_router_init()
{
  // Register CLIPS routers
# define CL_ADD_ROUTER(name) AddRouter((name), 20, cl_router_query, cl_router_print, cl_router_getc, cl_router_ungetc, cl_router_exit)
  CL_ADD_ROUTER("stdout");
  CL_ADD_ROUTER("wdisplay");
  CL_ADD_ROUTER("wclips");
  CL_ADD_ROUTER("werror");
//  CL_ADD_ROUTER("wdialog");
//  CL_ADD_ROUTER("wprompt");
//  CL_ADD_ROUTER("wwarning");
//  CL_ADD_ROUTER("wtrace");
//  CL_ADD_ROUTER("command");
# undef CL_ADD_ROUTER

  // Creating backend for storing routers strings
  cl_router_content = rb_str_new2("");
}

/**
 * Return the string that was captured from clips and
 * destroy it, so it's suitable for next use.
 */
VALUE cl_router_get_content_d()
{
  VALUE ret = cl_router_content;
  cl_router_content = rb_str_new2("");
  return ret;
}

/**
 * Setting which routers to capture
 */
int cl_router_query(char *name)
{
# define CL_CHECK_ROUTER(second)  if(strcmp((name), (second)) == 0) return TRUE
  CL_CHECK_ROUTER("stdout");
  CL_CHECK_ROUTER("wdisplay");
  CL_CHECK_ROUTER("wclips");
  CL_CHECK_ROUTER("werror");
//  CL_CHECK_ROUTER("wprompt");
//  CL_CHECK_ROUTER("wdialog");
//  CL_CHECK_ROUTER("wwarning");
//  CL_CHECK_ROUTER("wtrace");
//  CL_CHECK_ROUTER("command");
# undef CL_CHECK_ROUTER
  return FALSE;
}

/**
 * Capture output
 */
int cl_router_print(char *name, char *str)
{
  // Skip printing PS1 (prompt)
  if(strcmp("wclips", name) == 0) return TRUE;

  cl_router_content = rb_str_cat2(cl_router_content, str);
  return TRUE;
}

/**
 * Fake input
 */
int cl_router_getc(char *name)
{
  // So far, really fake, no input
  return 'f'; // Like fuck
}

/**
 * Ungetc wrap
 */
int cl_router_ungetc(int c, char *name)
{
  return c;
}

/**
 * Exit routers
 */
int cl_router_exit(int code)
{
  return code;
}
