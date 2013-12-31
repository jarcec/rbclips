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
#ifndef _RBFACT_H_
#define _RBFACT_H_

//! Clips::Fact
extern VALUE cl_cFact;

//! Clips::FactAddress
extern VALUE cl_cFactAddress;

//! Holds values stored inside Clips::Fact
struct _cl_sFactWrap
{
  void * ptr;
};
typedef struct _cl_sFactWrap cl_sFactWrap;

//! Specify family of methods should be created for fact
enum _cl_eMethodFamilies
{
  FACT_FAMILY_READ  = 1,
  FACT_FAMILY_WRITE = 2,
};

//! Clips::Fact::new
VALUE cl_fact_new(int, VALUE *, VALUE);

//! Clips::Fact::all
VALUE cl_fact_all(VALUE);

//! Clips::Fact::find
VALUE cl_fact_find(int, VALUE *, VALUE);

//! Clips::Fact\#initialize
VALUE cl_fact_initialize(VALUE, VALUE, VALUE);

//! Clips::Fact\#to_s
VALUE cl_fact_to_s(VALUE);

//! Clips::Fact\#clone
VALUE cl_fact_clone(VALUE);

//! Clips::Fact\#to_s
VALUE cl_fact_equal(VALUE, VALUE);

//! Clips::Fact\#slots (ordered only)
VALUE cl_fact_slots(VALUE);

//! Clips::Fact\#slot (nonordered only)
VALUE cl_fact_slot(VALUE, VALUE);

//! Clips::Fact\#set_slot (nonordered only)
VALUE cl_fact_set_slot(VALUE, VALUE, VALUE);

//! Clips::Fact\#name, template (get name or template (@template) from fact)
VALUE cl_fact_template(VALUE);

//! Clips::Fact\#save
VALUE cl_fact_save(VALUE);

//! Clips::Fact\#saved?
VALUE cl_fact_saved(VALUE);

//! Clips::Fact\#update
VALUE cl_fact_update(VALUE);

//! Clips::Fact\#destroy
VALUE cl_fact_destroy(VALUE);

//! Clips::Fact\#ordered?
VALUE cl_fact_ordered(VALUE);

//! Define instance methods for given fact
void cl_fact_define_instance_methods(VALUE, int);

//! Clips::FactAddress\#to_s
VALUE cl_factaddress_to_s(VALUE);

#endif // _RBFACT_H_
