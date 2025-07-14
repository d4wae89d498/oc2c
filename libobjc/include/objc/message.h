#ifndef OBJC_MESSAGE_H
# define OBJC_MESSAGE_H
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

int __check_if_nonnull_ptr(void *ptr);

# define objc_msgSendSuper(obj, ...)\
    (!__check_if_nonnull_ptr(obj) || !((struct objc_super *)obj)->super_class ? NULL : \
        class_getInstanceMethodCheck(((struct objc_super *)obj)->super_class, __VA_ARGS__)->imp((id)obj, __VA_ARGS__)\
    )

# define objc_msgSendSuper_fpret(obj, ...)\
    (!obj || !((struct objc_super *)obj)->isa ? NULL :\
        (double (*)(id, SEL, ...))(class_getInstanceMethodCheck(((struct objc_super *)obj)->isa, __VA_ARGS__)->imp)((id)obj, __VA_ARGS__)\
    )
# define objc_msgSendSuper_stret(ptr, obj, ...)\
    (void)(!((struct objc_super *)obj) || !((struct objc_super *)obj)->isa ? NULL :\
        ((void* (*)(void*, id, ...))(class_getInstanceMethodCheck(((struct objc_super *)obj)->isa, __VA_ARGS__)->imp))(ptr, (id)obj, __VA_ARGS__)\
    )



Method *class_getInstanceMethodCheck(Class isa, SEL sel, ...);
# ifdef __cplusplus
}
# endif
#endif // OBJC_MESSAGE_H