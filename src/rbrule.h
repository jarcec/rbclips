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
#ifndef _RBRULE_H_
#define _RBRULE_H_

//! Clips::Rule
extern VALUE cl_cRule;

//! Clips::Rule::Creator
extern VALUE cl_cRuleCreator;

//! Holds values stored inside Clips::Rule
struct _cl_sRuleWrap
{
  void * ptr;
};
typedef struct _cl_sRuleWrap cl_sRuleWrap;

//! Holds values stored inside Clips::Rule::Creator
struct _cl_sRuleCreatorWrap
{
  VALUE ptr;
  unsigned counter;
  int not_block;    // Boolean if we're inside not block
};
typedef struct _cl_sRuleCreatorWrap cl_sRuleCreatorWrap;

/** 
 * List with rule block types - AND, OR, NOT
 */
enum RULES_BLOCK_TYPES
{
  BLOCK_AND,
  BLOCK_OR,
  BLOCK_NOT,
};

/* Function list */

//! Clips::Rule::new
VALUE cl_rule_new(int, VALUE *, VALUE);

//! Clips::Rule::load
VALUE cl_rule_load(VALUE, VALUE);

//! Clips::Rule::all
VALUE cl_rule_all(VALUE);

//! Clips::Rule\#initialize
VALUE cl_rule_initialize(VALUE, VALUE);

//! Clips::Rule\#to_s
VALUE cl_rule_to_s(VALUE);

//! Clips::Rule\#save
VALUE cl_rule_save(VALUE);

//! Clips::Rule\#destroy!
VALUE cl_rule_destroy(VALUE);

//! Clips::Rule\#update
VALUE cl_rule_update(VALUE);

//! Clips::Rule\#equal
VALUE cl_rule_equal(VALUE, VALUE);

//! Clips::Rule::Creator\#initialize
VALUE cl_rule_creator_initialize(VALUE);

//! Clips::Rule::Creator\#pattern
VALUE cl_rule_creator_pattern(int, VALUE *, VALUE);

//! Clips::Rule::Creator\#assert
VALUE cl_rule_creator_assert(int, VALUE *, VALUE);

//! Clips::Rule::Creator\#retract
VALUE cl_rule_creator_retract(int, VALUE *, VALUE);

//! Clips::Rule::Creator\#rhs
VALUE cl_rule_creator_rhs(VALUE, VALUE);

//! Clips::Rule::Creator\#rcall
VALUE cl_rule_creator_rcall(int, VALUE *, VALUE);

//! Clips::Rule::Creator\#or
VALUE cl_rule_creator_or(VALUE);

//! Clips::Rule::Creator\#and
VALUE cl_rule_creator_and(VALUE);

//! Clips::Rule::Creator\#not
VALUE cl_rule_creator_not(VALUE);


#endif // _RBRULE_H_
