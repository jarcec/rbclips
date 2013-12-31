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
#ifndef _RBROUTER_H_
#define _RBROUTER_H_

/* Function list */

//! Register CLIPS router and set it up
void cl_router_init();

//! Return wheather we have something from werror router
int cl_router_werror();

//! Get the output from router (and as side effect destroy it internally)
VALUE cl_router_get_content_d();

#endif // _RBROUTER_H_
