#ifndef _RBCONSTRAINT_H_
#define _RBCONSTRAINT_H_

#include <ruby.h>

//! Clips::Constraint
extern VALUE cl_cConstraint;

//! Clips::Constraint::Creator
extern VALUE cl_cConstraintCreator;

//! Holds values stored inside ruby object
struct _cl_sConstraintCreatorWrap
{
  VALUE ptr;
};
typedef struct _cl_sConstraintCreatorWrap cl_sConstraintCreatorWrap;

/* Function list */

//! Clips::Constraint::new
VALUE cl_constraint_initialize(int, VALUE *, VALUE);

//! Clips::Constraint\#to_s
VALUE cl_constraint_to_s(VALUE);

//! Clips::Constraint::Creator\#initialize
VALUE cl_constraint_creator_initialize(VALUE);

//! Clips::Constraint::Creator\#type
VALUE cl_constraint_creator_type(VALUE, VALUE);

//! Clips::Constraint::Creator\#values
VALUE cl_constraint_creator_values(VALUE, VALUE);

//! Clips::Constraint::Creator\#range
VALUE cl_constraint_creator_range(VALUE, VALUE);

//! Clips::Constraint::Creator\#cardinality
VALUE cl_constraint_creator_cardinality(VALUE, VALUE);

#endif // _RBCONSTRAINT_H_
