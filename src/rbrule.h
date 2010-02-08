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
};
typedef struct _cl_sRuleCreatorWrap cl_sRuleCreatorWrap;

/* Function list */

//! Clips::Rule::new
VALUE cl_rule_new(int, VALUE *, VALUE);

//! Clips::Rule\#initialize
VALUE cl_rule_initialize(VALUE, VALUE);

//! Clips::Rule\#to_s
VALUE cl_rule_to_s(VALUE);

//! Clips::Rule\#save
VALUE cl_rule_save(VALUE);

//! Clips::Rule\#destroy!
VALUE cl_rule_destroy(VALUE);

//! Clips::Rule::Creator\#initialize
VALUE cl_rule_creator_initialize(VALUE);

//! Clips::Rule::Creator\#pattern
VALUE cl_rule_creator_pattern(int, VALUE *, VALUE);

//! Clips::Rule::Creator\#rhs
VALUE cl_rule_creator_rhs(VALUE, VALUE);


#endif // _RBRULE_H_
