#ifndef _RBFACT_H_
#define _RBFACT_H_

//! Clips::Fact
extern VALUE cl_cFact;

//! Holds values stored inside Clips::Fact
struct _cl_sFactWrap
{
  void * ptr;
};
typedef struct _cl_sFactWrap cl_sFactWrap;

//! Clips::Fact::new
VALUE cl_fact_new(int, VALUE *, VALUE);

//! Clips::Fact\#initialize
VALUE cl_fact_initialize(VALUE, VALUE, VALUE);

//! Clips::Fact\#to_s
VALUE cl_fact_to_s(VALUE);

//! Clips::Fact\#save
VALUE cl_fact_save(VALUE);

//! Clips::Fact\#destroy
VALUE cl_fact_destroy(VALUE);

//! Clips::Fact\#ordered?
VALUE cl_fact_ordered(VALUE);

#endif // _RBFACT_H_
