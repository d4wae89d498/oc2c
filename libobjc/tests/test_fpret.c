#include <stdio.h>
#include "objc/runtime.h"
#include "objc/message.h"

double double_method(id self, SEL _cmd, ...) {
    double result = 3.14159;
    printf("double_method: returning %f\n", result);
    return result;
}

int main() {
    SEL sel_double = sel_registerName("double");
    Class MyClass = objc_allocateClassPair("FpretClass", Object_class, 0);
    class_addMethod(MyClass, sel_double, (IMP)double_method, "d@:");
    objc_registerClassPair(MyClass);
    id obj = objc_alloc(MyClass);
    objc_init(obj);
    double result = 0.1;// objc_msgSend_fpret(obj, sel_double);
    printf("Received: %f\n", result);
    objc_dealloc(obj);
    return 0;
} 