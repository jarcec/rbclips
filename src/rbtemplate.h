#ifndef _RBTEMPLATE_H_
#define _RBTEMPLATE_H_

//! Clips::Template
extern VALUE cl_cTemplate;

/* Function list */

//! Clips::Template\#initialize
VALUE cl_template_initialize(int, VALUE *, VALUE);

//! Clips::Template\#to_s
VALUE cl_template_to_s(VALUE);

#endif // _RBTEMPLATE_H_
