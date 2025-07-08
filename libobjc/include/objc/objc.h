#ifndef OBJC_OBJC_H
# define OBJC_OBJC_H
# ifdef __cplusplus
extern "C" {
# endif
typedef struct objc_object *id;
typedef struct objc_class *Class;
typedef struct objc_selector *SEL;
typedef void *(*IMP)(id, SEL, ...);
# ifdef __cplusplus
}
# endif
#endif // OBJC_OBJC_H 