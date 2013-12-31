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
#ifndef _RBGENERIC_H_
#define _RBGENERIC_H_

/* Function list */

//! Return string (CLIPS) reprezentation of given ID
const char const * rb_generic_clipstype_str(ID);

//! Retrun string CLIPS reprezentation of given slot value
const char const * rb_generic_slot_value(VALUE);

//! Check if given string is valid clips symbol (regexp based)
int cl_generic_check_clips_symbol(VALUE);

//! Convert one value from multifield DATA_OBJECT to VALUE
VALUE cl_generic_convert_dataobject_mf(void *, int);

//! Convert CLIPS DATA_OBJECT to ruby object
VALUE cl_generic_convert_dataobject(DATA_OBJECT);

#endif // _RBGENERIC_H_
