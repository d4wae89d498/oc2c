#include <stdio.h>
#include "objc/runtime.h"

int main() {
    Class MyClass = objc_allocateClassPair("RegClass", Object_class, 0);
    objc_registerClassPair(MyClass);
    Class found = objc_getClass("RegClass");
    printf("Found class: %s\n", class_getName(found));
    printf("Superclass: %s\n", class_getName(class_getSuperclass(found)));
    return found == MyClass ? 0 : 1;
} 