#ifndef _RBROUTER_H_
#define _RBROUTER_H_

/* Function list */

//! Register CLIPS router and set it up
void cl_router_init();

//! Get the output from router (and as side effect destroy it internally)
VALUE cl_router_get_content_d();

#endif // _RBROUTER_H_
