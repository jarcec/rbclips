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

//! Clips::Fact\#name, template (get name or template (@template) from fact)
VALUE cl_fact_template(VALUE);

//! Clips::Fact\#save
VALUE cl_fact_save(VALUE);

//! Clips::Fact\#update
VALUE cl_fact_update(VALUE);

//! Clips::Fact\#destroy
VALUE cl_fact_destroy(VALUE);

//! Clips::Fact\#ordered?
VALUE cl_fact_ordered(VALUE);

//! Clips::FactAddress\#to_s
VALUE cl_factaddress_to_s(VALUE);

#endif // _RBFACT_H_
