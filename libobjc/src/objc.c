#include "objc/message.h"
#include "objc/objc.h"
#include "objc/runtime.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include <stddef.h>
#include <stdarg.h>

#include "./../../_strdup.h"

#define MAX_CLASSES 128
#define MAX_SELECTORS 256
#define MAX_METHODS 128

static Class class_table[MAX_CLASSES];
static int class_count = 0;

static struct objc_selector selector_table[MAX_SELECTORS];
static int selector_count = 0;


SEL sel_registerName(const char *name) {
    for (int i = 0; i < selector_count; ++i) {
        if (strcmp(selector_table[i].name, name) == 0)
            return &selector_table[i];
    }
    if (selector_count < MAX_SELECTORS) {
        selector_table[selector_count].name = _strdup(name);
        return &selector_table[selector_count++];
    }
    return NULL;
}


Method *class_getInstanceMethodCheck(Class isa, SEL sel, ...)
{

    Method *m = class_getInstanceMethod(isa, sel);
    if (m)
        return m;
    fprintf(stderr, "Unrecognized selector '%s'\n", sel_getName(sel));
    abort();
    return NULL;
}

const char *sel_getName(SEL sel) {
    return sel ? sel->name : NULL;
}

// --- Class registration ---
Class objc_allocateClassPair(const char *name, Class super_class, size_t extra_bytes) {
    Class cls = (Class)calloc(1, sizeof(struct objc_class) + extra_bytes);
    cls->name = _strdup(name);
    cls->super_class = super_class;
    cls->meta_class = NULL;
    cls->method_list = calloc(MAX_METHODS, sizeof(Method));
    cls->method_count = 0;
    cls->instance_size = sizeof(struct objc_object) + extra_bytes;
    return cls;
}

void objc_registerClassPair(Class cls) {
    if (class_count < MAX_CLASSES) {
        class_table[class_count++] = cls;
    }
}

Class objc_getClass(const char *name) {
    for (int i = 0; i < class_count; ++i) {
        if (strcmp(class_table[i]->name, name) == 0)
            return class_table[i];
    }
    return NULL;
}

Class objc_getMetaClass(const char *name) {
    Class cls = objc_getClass(name);
    return cls ? cls->meta_class : NULL;
}

// --- Method registration ---
void class_addMethod(Class cls, SEL sel, IMP imp, const char *types) {
    if (cls->method_count < MAX_METHODS) {
        cls->method_list[cls->method_count].sel = sel;
        cls->method_list[cls->method_count].imp = imp;
        cls->method_list[cls->method_count].types = types;
        cls->method_count++;
    }
}
Method *_class_getInstanceMethod(Class cls, SEL sel, ...) {
    return class_getInstanceMethod(cls, sel);
}
Method *class_getInstanceMethod(Class cls, SEL sel) {
    while (cls) {
        for (int i = 0; i < cls->method_count; ++i) {
            if (cls->method_list[i].sel == sel)
                return &cls->method_list[i];
        }
        cls = cls->super_class;
    }
    return NULL;
}

Method *class_getClassMethod(Class cls, SEL sel) {
    if (cls && cls->meta_class)
        return class_getInstanceMethod(cls->meta_class, sel);
    return NULL;
}

struct objc_super {
    id receiver;
    Class super_class;
};

const char *class_getName(Class cls) {
    return cls ? cls->name : NULL;
}

Class class_getSuperclass(Class cls) {
    return cls ? cls->super_class : NULL;
}

id objc_alloc(Class cls) {
    id obj = (id)calloc(1, cls->instance_size);
    obj->isa = cls;
    return obj;
}

id objc_init(id obj) {
    //  TODO
    return obj;
}

void objc_dealloc(id obj) {
    free(obj);
}

static struct objc_class _Object_class = {
    .name = "NSObject",
    .super_class = NULL,
    .meta_class = NULL,
    .method_list = NULL,
    .method_count = 0,
    .instance_size = sizeof(struct objc_object)
};

Class Object_class = &_Object_class; 


int __check_if_nonnull_ptr(void *ptr)
{
    return ptr != NULL;
}