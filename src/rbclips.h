#ifndef _RBCLIPS_H_
#define _RBCLIPS_H_

//! rbClips main module
extern VALUE cl_mClips;

//! Holds symbols that are used in many places across rbclips
struct _cl_sIds
{
  // General
  ID any;
  ID unknown;

  // Ruby methods
  ID to_s;
  ID to_sym;
  ID begin;
  ID end;
  ID clear;
  ID size;
  ID update;
  ID eqq;
  ID ordered;
  ID saved;
  ID clone;
  ID load;

  // Mostly hash keys
  ID type;
  ID values;
  ID range;
  ID cardinality;
  ID name;
  ID slots;
  ID multislot;
  ID default_;
  ID default_dynamic;
  ID constraint;
  ID none;
  ID derive;

  // CLIPS build-in types
  ID symbol;
  ID string;
  ID lexeme;
  ID integer;
  ID float_;
  ID number;
  ID instance_name;
  ID instance_address;
  ID instance;
  ID external_address;
  ID fact_address;
};
typedef struct _cl_sIds cl_sIds;

//! Internal list of general used IDs
extern cl_sIds cl_vIds;

// Some C "magic"
# ifndef false
#   define false 0
# endif
# ifndef true
#  define true 1
# endif

//! Get string representation of given object
#define CL_TO_S(v)        rb_funcall( (v), cl_vIds.to_s, 0)

//! Get C string of given VALUE (wrapper)
#define CL_STR(v)         STR2CSTR( CL_TO_S(v) )

//! Get C string of given class of given VALUE (wrapper)
#define CL_STR_CLASS(v)   CL_STR(rb_obj_class(v))

//! Call update object inner structures
#define CL_UPDATE(v)      rb_funcall( (v), cl_vIds.update, 0)

//! Return escaped string - mean that string object have quotes
#define CL_STR_ESCAPE(source) (TYPE(source) == T_STRING) ? CL_STR(rb_sprintf("\"%s\"", CL_STR(source))) : CL_STR(source)

/** Easy writing equal methods
 * Macro for easy writing equal methods
 */
#define CL_EQUAL_DEFINE           VALUE aa, bb, ret
#define CL_EQUAL_CHECK_IV(what)   aa = rb_iv_get(a, (what)); \
                                  bb = rb_iv_get(b, (what)); \
                                  ret = rb_equal(aa, bb); \
                                  if(TYPE(ret) != T_TRUE) return Qfalse
#define CL_EQUAL_DEFINE_WRAP(s)   s *aaa = DATA_PTR(a); \
                                  s *bbb = DATA_PTR(b)
#define CL_EQUAL_CHECK_PTR        if(!aaa || !bbb || aaa->ptr != bbb->ptr) return false

#endif // _RBCLIPS_H_
