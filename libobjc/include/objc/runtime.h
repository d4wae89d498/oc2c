#ifndef OBJC_RUNTIME_H
# define OBJC_RUNTIME_H
# include <stddef.h>
# ifdef __cplusplus
extern "C" {
# endif
# include "./objc.h"

typedef struct objc_method {
    SEL sel;
    IMP imp;
    const char *types;
} Method;

struct objc_selector {
    const char *name;
};

struct objc_object {
    Class isa;
};

struct objc_class {
    const char *name;
    Class super_class;
    Class meta_class;
    Method *method_list;
    int method_count;
    unsigned long long instance_size;
};

SEL sel_registerName(const char *name);
const char *sel_getName(SEL sel);

Class objc_allocateClassPair(const char *name, Class super_class, size_t  extra_bytes);
void objc_registerClassPair(Class cls);
Class objc_getClass(const char *name);
Class objc_getMetaClass(const char *name);

void class_addMethod(Class cls, SEL sel, IMP imp, const char *types);
Method *class_getInstanceMethod(Class cls, SEL sel);
Method *class_getClassMethod(Class cls, SEL sel);

const char *class_getName(Class cls);
Class class_getSuperclass(Class cls);

extern Class Object_class;

id objc_alloc(Class cls);
id objc_init(id obj);
void objc_dealloc(id obj);

// TODO:
id class_createInstance(Class, size_t);
void object_dispose(id);

# ifdef __cplusplus
}
# endif
#endif // OBJC_H 