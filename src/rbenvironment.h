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
#ifndef _RBENVIRONMNET_H_
#define _RBENVIRONMNET_H_

//! Clips::Environment
extern VALUE cl_cEnvironment;

//! Internal global list of created (and existing) environments
extern VALUE cl_vEnvironments;

//! Holds values stored inside ruby object
struct _cl_sEnvironmentWrap
{
  void *ptr;
};
typedef struct _cl_sEnvironmentWrap cl_sEnvironmentWrap;

/* Function list */

//! Environment::new
VALUE cl_environment_new(VALUE);

//! Environment\#to_s
VALUE cl_environment_to_s(VALUE);

//! Environment::all
VALUE cl_environment_all();

//! Environment::current
VALUE cl_environment_current();

//! Environment\#set_current
VALUE cl_environment_set_current(VALUE);

//! Environment\#dup, Environmnet\#clone
VALUE cl_environment_clone(VALUE);

//! Environment\#eql?, Environmnet\#equal?, Environmnet\#==
VALUE cl_environment_equal(VALUE, VALUE);

//! Environment\#valid?
VALUE cl_environment_valid(VALUE);

//! Environment\#destroy!
VALUE cl_environment_destroy(VALUE);

#endif // _RBENVIRONMNET_H_
