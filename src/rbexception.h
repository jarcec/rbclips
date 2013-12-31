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
#ifndef _RBEXCEPTION_H_
#define _RBEXCEPTION_H_

//! Clips::Excetion
extern VALUE cl_eException;

//! Clips::ArgumentError
extern VALUE cl_eArgError;

//! Clips::UsageError
extern VALUE cl_eUseError;

//! Clips::InUseException
extern VALUE cl_eInUseError;

//! Clips::InternalError
extern VALUE cl_eInternError;

//! Clips::NotImplementedError
extern VALUE cl_eNotImplError;

#endif // _RBEXCEPTION_H_
