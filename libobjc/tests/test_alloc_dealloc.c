#include <stdio.h>
#include "objc/runtime.h"

int main() {
    Class MyClass = objc_allocateClassPair("AllocClass", Object_class, 0);
    objc_registerClassPair(MyClass);
    id obj = objc_alloc(MyClass);
    printf("Allocated object at %p\n", (void*)obj);
    objc_dealloc(obj);
    printf("Deallocated object\n");
    return 0;
} 