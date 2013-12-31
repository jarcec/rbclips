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
#ifndef _RBCONSTRAINT_H_
#define _RBCONSTRAINT_H_

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

//! Clips::Constraint\#initialize
VALUE cl_constraint_initialize(int, VALUE *, VALUE);

//! Clips::Constraint\#to_s
VALUE cl_constraint_to_s(VALUE);

//! Clips::Constraint\#equal
VALUE cl_constraint_equal(VALUE, VALUE);

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
