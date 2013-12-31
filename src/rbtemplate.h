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
#ifndef _RBTEMPLATE_H_
#define _RBTEMPLATE_H_

//! Clips::Template
extern VALUE cl_cTemplate;

//! Clips::Template::Creator
extern VALUE cl_cTemplateCreator;

//! Holds values stored inside Clips::Template
struct _cl_sTemplateWrap
{
  void * ptr;
};
typedef struct _cl_sTemplateWrap cl_sTemplateWrap;

//! Holds values stored inside Clips::Template::Creator
struct _cl_sTemplateCreatorWrap
{
  VALUE ptr;
};
typedef struct _cl_sTemplateCreatorWrap cl_sTemplateCreatorWrap;

/* Function list */

//! Clips::Template::new
VALUE cl_template_new(int, VALUE *, VALUE);

//! Clips::Template::load
VALUE cl_template_load(VALUE, VALUE);

//! Clips::Template::all
VALUE cl_template_all(VALUE);

//! Clips::Template\#initialize
VALUE cl_template_initialize(int, VALUE *, VALUE);

//! Clips::Template\#to_s
VALUE cl_template_to_s(VALUE);

//! Clips::Template\#clone
VALUE cl_template_clone(VALUE);

//! Clips::Template\#equal
VALUE cl_template_equal(VALUE, VALUE);

//! Clips::Template\#save
VALUE cl_template_save(VALUE);

//! Clips::Template\#saved?
VALUE cl_template_saved(VALUE);

//! Clips::Template\#destroy!
VALUE cl_template_destroy(VALUE);

//! Clips::Template\#update
VALUE cl_template_update(VALUE);

//! Clips::Template::Creator\#initialize
VALUE cl_template_creator_initialize(VALUE);

//! Clips::Template::Creator\#slot
VALUE cl_template_creator_slot(int, VALUE *, VALUE);

#endif // _RBTEMPLATE_H_
