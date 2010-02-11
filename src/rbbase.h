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

