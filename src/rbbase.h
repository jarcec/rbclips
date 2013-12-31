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
#ifndef _RBBASE_H_
#define _RBBASE_H_

//! Clips::Base
extern VALUE cl_cBase;    

/* Function list */

//! Base::insert_command
VALUE cl_base_insert_command(VALUE, VALUE);

//! Base::run
VALUE cl_base_run(int, VALUE *, VALUE);

//! Base::reset
VALUE cl_base_reset(VALUE);

//! Base::clear
VALUE cl_base_clear(VALUE);

#endif // _RBBASE_H_

