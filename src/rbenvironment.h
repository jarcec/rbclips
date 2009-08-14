#ifndef _RBENVIRONMNET_H_
#define _RBENVIRONMNET_H_

//! Clips::Environment
extern VALUE cl_cEnvironment;

//! Internal global list of created (and existing) environments
extern VALUE cl_vEnvironments;

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

//! Environment\#all
VALUE cl_environment_all();

//! Environment\#current
VALUE cl_environment_current();

//! Environment\#set_current
VALUE cl_environment_set_current(VALUE);

#endif // _RBENVIRONMNET_H_
