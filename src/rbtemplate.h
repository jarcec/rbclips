#ifndef _RBTEMPLATE_H_
#define _RBTEMPLATE_H_

//! Clips::Template
extern VALUE cl_cTemplate;

//! Clips::Template::Creator
extern VALUE cl_cTemplateCreator;

//! Holds values stored inside ruby object
struct _cl_sTemplateCreatorWrap
{
  VALUE ptr;
};
typedef struct _cl_sTemplateCreatorWrap cl_sTemplateCreatorWrap;

/* Function list */

//! Clips::Template\#initialize
VALUE cl_template_initialize(int, VALUE *, VALUE);

//! Clips::Template\#to_s
VALUE cl_template_to_s(VALUE);

//! Clips::Template\#equal
VALUE cl_template_equal(VALUE, VALUE);

//! Clips::Template::Creator\#initialize
VALUE cl_template_creator_initialize(VALUE);

//! Clips::Template::Creator\#slot
VALUE cl_template_creator_slot(int, VALUE *, VALUE);

#endif // _RBTEMPLATE_H_
