#ifndef _RBEXCEPTION_H_
#define _RBEXCEPTION_H_

//! Clips::Excetion
extern VALUE cl_eException;

//! Clips::ArgumentError
extern VALUE cl_eArgError;

//! Clips::UsageError
extern VALUE cl_eUseError;

//! Get C string of given VALUE (wrapper)
#define CL_STR(v)         STR2CSTR( rb_funcall( (v), cl_vIds.to_s, 0 ) )

//! Get C string of given class of given VALUE (wrapper)
#define CL_STR_CLASS(v)   CL_STR(rb_obj_class(v))

#endif // _RBEXCEPTION_H_
