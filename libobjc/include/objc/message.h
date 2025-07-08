#ifndef OBJC_H
# define OBJC_H
# include <string.h>
# include "./runtime.h"
# ifdef __cplusplus
extern "C" {
# endif
# define objc_msgSend(obj, ...)\
    (!obj || !obj->isa ? NULL :\
        class_getInstanceMethodCheck(obj->isa, __VA_ARGS__)->imp(obj, __VA_ARGS__)\
    )
# define objc_msgSend_fpret(obj, ...)\
    (!obj || !obj->isa ? NULL :\
        (double (*)(id, SEL, ...))(class_getInstanceMethodCheck(obj->isa, __VA_ARGS__)->imp)(obj, __VA_ARGS__)\
    )
# define objc_msgSend_stret(ptr, obj, ...)\
    (void)(!obj || !obj->isa ? NULL :\
        ((void* (*)(void*, id, ...))(class_getInstanceMethodCheck(obj->isa, __VA_ARGS__)->imp))(ptr, obj, __VA_ARGS__)\
    )

Method *class_getInstanceMethodCheck(Class isa, SEL sel, ...);
# ifdef __cplusplus
}
# endif
#endif // OBJC_H 