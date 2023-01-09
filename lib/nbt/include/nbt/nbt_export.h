// found at internet.
#ifndef NBT_EXPORT_H
#define NBT_EXPORT_H

#ifdef NBT_STATIC_DEFINE
#  define NBT_EXPORT
#  define NBT_NO_EXPORT
#else
#  ifndef NBT_EXPORT
#    ifdef nbt___EXPORTS
/* We are building this library */
#      define NBT_EXPORT
#    else
/* We are using this library */
#      define NBT_EXPORT
#    endif
#  endif

#  ifndef NBT_NO_EXPORT
#    define NBT_NO_EXPORT
#  endif
#endif

#ifndef NBT_DEPRECATED
#  define NBT_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef NBT_DEPRECATED_EXPORT
#  define NBT_DEPRECATED_EXPORT NBT_EXPORT NBT_DEPRECATED
#endif

#ifndef NBT_DEPRECATED_NO_EXPORT
#  define NBT_DEPRECATED_NO_EXPORT NBT_NO_EXPORT NBT_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef NBT_NO_DEPRECATED
#    define NBT_NO_DEPRECATED
#  endif
#endif

#endif