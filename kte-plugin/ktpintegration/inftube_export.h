#ifndef INFTUBE_EXPORT_H
#define INFTUBE_EXPORT_H

#include <kdemacros.h>

#ifndef INFTUBE_EXPORT
# if defined(MAKE_INFTUBE_LIB)
   // We are building this library
#  define INFTUBE_EXPORT KDE_EXPORT
# else
   // We are using this library
#  define INFTUBE_EXPORT KDE_IMPORT
# endif
#endif

# ifndef INFTUBE_EXPORT_DEPRECATED
#  define INFTUBE_EXPORT_DEPRECATED KDE_DEPRECATED INFTUBE_EXPORT
# endif

#endif