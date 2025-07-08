#include <stdio.h>
#include "objc/runtime.h"
#include "objc/message.h"

struct objc_super {
    id receiver;
    Class super_class;
};

void *base_speak(id self, SEL _cmd, ...) {
    printf("Base speaks!\n");
    return NULL;
}

void *sub_speak(id self, SEL _cmd, ...) {
    printf("Sub: calling super...\n");
    struct objc_super sup = { self, class_getSuperclass(self->isa) };
    objc_msgSendSuper(&sup, _cmd);
    printf("Sub done.\n");
    return NULL;
}

int main() {
    SEL sel_speak = sel_registerName("speak");
    Class BaseClass = objc_allocateClassPair("BaseClass", Object_class, 0);
    class_addMethod(BaseClass, sel_speak, (IMP)base_speak, "v@:");
    objc_registerClassPair(BaseClass);
    Class SubClass = objc_allocateClassPair("SubClass", BaseClass, 0);
    class_addMethod(SubClass, sel_speak, (IMP)sub_speak, "v@:");
    objc_registerClassPair(SubClass);
    id sub = objc_alloc(SubClass);
    printf("Calling [sub speak]:\n");
    objc_msgSend(sub, sel_speak);
    objc_dealloc(sub);
    return 0;
} 