#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "objc/runtime.h"
#include "objc/message.h"

void *base_speak(id self, SEL _cmd, ...) {
    printf("Base speaks!\n");
    return NULL;
}

void *sub_speak(id self, SEL _cmd, ...) {
    printf("Sub speaks!\n");
    return NULL;
}

void *base_only(id self, SEL _cmd, ...) {
    printf("Base only method!\n");
    return NULL;
}

int main() {
    SEL sel_speak = sel_registerName("speak");
    SEL sel_baseonly = sel_registerName("baseOnly");
    assert(sel_speak != NULL);
    assert(sel_baseonly != NULL);
    
    Class BaseClass = objc_allocateClassPair("BaseClass", Object_class, 0);
    assert(BaseClass != NULL);
    class_addMethod(BaseClass, sel_speak, (IMP)base_speak, "v@:");
    class_addMethod(BaseClass, sel_baseonly, (IMP)base_only, "v@:");
    objc_registerClassPair(BaseClass);
    
    Class SubClass = objc_allocateClassPair("SubClass", BaseClass, 0);
    assert(SubClass != NULL);
    assert(class_getSuperclass(SubClass) == BaseClass);
    class_addMethod(SubClass, sel_speak, (IMP)sub_speak, "v@:");
    objc_registerClassPair(SubClass);
    
    id base = objc_alloc(BaseClass);
    assert(base != NULL);
    assert(base->isa == BaseClass);
    printf("BaseClass: ");
    objc_msgSend(base, sel_speak);
    objc_msgSend(base, sel_baseonly);
    
    id sub = objc_alloc(SubClass);
    assert(sub != NULL);
    assert(sub->isa == SubClass);
    assert(class_getSuperclass(sub->isa) == BaseClass);
    printf("SubClass: ");
    objc_msgSend(sub, sel_speak);
    printf("SubClass (baseOnly): ");
    objc_msgSend(sub, sel_baseonly);
    
    objc_dealloc(base);
    objc_dealloc(sub);
    printf("test_inheritance: PASSED\n");
    return 0;
} 