#include <stdio.h>
#include "objc/runtime.h"
#include "objc/message.h"

void *class_foo(id self, SEL _cmd, ...) {
    printf("Class method foo called!\n");
    return NULL;
}

void *instance_bar(id self, SEL _cmd, ...) {
    printf("Instance method bar called!\n");
    return NULL;
}

int main() {
    SEL sel_foo = sel_registerName("foo");
    SEL sel_bar = sel_registerName("bar");
    Class MyClass = objc_allocateClassPair("MetaTest", Object_class, 0);
    class_addMethod(MyClass, sel_bar, (IMP)instance_bar, "v@:");
    MyClass->meta_class = objc_allocateClassPair("MetaTestMeta", NULL, 0);
    class_addMethod(MyClass->meta_class, sel_foo, (IMP)class_foo, "v@:");
    objc_registerClassPair(MyClass);
    objc_registerClassPair(MyClass->meta_class);
    id obj = objc_alloc(MyClass);
    printf("Instance: ");
    objc_msgSend(obj, sel_bar);
    printf("Class: ");
    Method *cm = class_getClassMethod(MyClass, sel_foo);
    if (cm) cm->imp((id)MyClass, sel_foo);
    else printf("Class method not found!\n");
    objc_dealloc(obj);
    return 0;
} 