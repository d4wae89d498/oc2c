#include <stdio.h>
#include "objc/runtime.h"

void *multi_arg_method(id self, SEL _cmd, int a, float b, const char *c) {
    printf("multi_arg_method: a=%d, b=%f, c=%s\n", a, b, c);
    return NULL;
}

int main() {
    SEL sel_multi = sel_registerName("multi:");
    Class MyClass = objc_allocateClassPair("MultiArgClass", Object_class, 0);
    class_addMethod(MyClass, sel_multi, (IMP)multi_arg_method, "v@:if*");
    objc_registerClassPair(MyClass);
    id obj = objc_alloc(MyClass);
    objc_init(obj);
    // Cast IMP to correct type for call
    typedef void *(*MultiArgIMP)(id, SEL, int, float, const char *);
    MultiArgIMP imp = (MultiArgIMP)multi_arg_method;
    imp(obj, sel_multi, 42, 3.14f, "hello");
    objc_dealloc(obj);
    return 0;
} 