#ifndef DECLSPEC_H
#define DECLSPEC_H
#include <QtCore/qglobal.h>

#if defined(GIT2QT_LIBRARY)
#  define GIT2QT_EXPORT Q_DECL_EXPORT
#else
#  define GIT2QT_EXPORT Q_DECL_IMPORT
#endif

#endif // DECLSPEC_H
