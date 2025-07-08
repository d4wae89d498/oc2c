#include <stdio.h>
#include <assert.h>
#include "objc/runtime.h"
#include "objc/message.h"

void *myclass_hello(id self, SEL _cmd, ...) {
    printf("Hello from MyClass!\n");
    return NULL;
}

int main() {
    SEL sel_hello = sel_registerName("hello");
    assert(sel_hello != NULL);
    assert(strcmp(sel_getName(sel_hello), "hello") == 0);
    
    Class MyClass = objc_allocateClassPair("MyClass", Object_class, 0);
    assert(MyClass != NULL);
    
    class_addMethod(MyClass, sel_hello, (IMP)myclass_hello, "v@:");
    objc_registerClassPair(MyClass);
    
    id obj = objc_alloc(MyClass);
    assert(obj != NULL);
    assert(obj->isa == MyClass);
    
    objc_init(obj);
    objc_msgSend(obj, sel_hello);
    
    assert(strcmp(class_getName(obj->isa), "MyClass") == 0);
    assert(class_getSuperclass(obj->isa) == Object_class);
    assert(strcmp(class_getName(class_getSuperclass(obj->isa)), "Object") == 0);
    
    printf("Class name: %s\n", class_getName(obj->isa));
    printf("Superclass: %s\n", class_getName(class_getSuperclass(obj->isa)));
    
    objc_dealloc(obj);
    printf("test_basic: PASSED\n");
    return 0;
} 