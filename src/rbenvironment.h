#ifndef _RBENVIRONMNET_H_
#define _RBENVIRONMNET_H_

//! Clips::Environment
extern VALUE cl_cEnvironment;    

//! Holds values stored inside ruby object
struct _cl_sEnvironmentWrap
{
  void *ptr;
};
typedef struct _cl_sEnvironmentWrap cl_sEnvironmentWrap;

/* Function list */

//! Environment\#new
VALUE cl_environment_new(VALUE);

//! Environment\#to_s
VALUE cl_environment_to_s(VALUE);

#endif // _RBENVIRONMNET_H_
