#ifndef KANOOPGIT_H
#define KANOOPGIT_H
#include <QtCore/qglobal.h>

#if defined(KANOOP_GIT)
#  define KANOOPGIT_EXPORT Q_DECL_EXPORT
#else
#  define KANOOPGIT_EXPORT Q_DECL_IMPORT
#endif


#endif // KANOOPGIT_H
