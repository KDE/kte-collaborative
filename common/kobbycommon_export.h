#ifndef KOBBYCOMMON_EXPORT_H
#define KOBBYCOMMON_EXPORT_H

#include <kdemacros.h>

#ifndef KOBBYCOMMON_EXPORT
# if defined(MAKE_KOBBYCOMMON_LIB)
   // We are building this library
#  define KOBBYCOMMON_EXPORT KDE_EXPORT
# else
   // We are using this library
#  define KOBBYCOMMON_EXPORT KDE_IMPORT
# endif
#endif

# ifndef KOBBYCOMMON_EXPORT_DEPRECATED
#  define KOBBYCOMMON_EXPORT_DEPRECATED KDE_DEPRECATED KOBBYCOMMON_EXPORT
# endif

#endif