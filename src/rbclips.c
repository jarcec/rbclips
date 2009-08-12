#include "rbclips.h"

// Definitons
VALUE cl_mClips;
VALUE cl_cBase;

/** Ruby module initialization
 *
 * Create and register all ruby modules and classes
 */
void Init_rbclips()
{
  // Module definition
  cl_mClips = rb_define_module("Clips");

  // Clips::Base
  cl_cBase = rb_define_class_under(cl_mClips, "Base", rb_cObject);
}
