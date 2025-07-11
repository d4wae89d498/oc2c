#define _GNU_SOURCE
#include <objc/runtime.h>
#include <objc/message.h>
#include <stdio.h>
#include <stdlib.h>



struct MyRawObject {
    void *isa;
    int x;
    int y;
};

void myMethod(struct MyRawObject * self, SEL _cmd) {
    printf("myMethod called! x = %d, y = %d\n", self->x, self->y);
}

int main() {
    const char *className = "MyDirtyClass";

    // Allocate class with space for 2 ints after isa (but no ivars formally declared)
    Class cls = objc_allocateClassPair(NULL, className, sizeof(struct MyRawObject) - sizeof(void *));
    objc_registerClassPair(cls);

    // Add method (no type encoding safety)
    class_addMethod(cls, sel_registerName("doSomething"), (IMP)myMethod, "");//, "v@:");

    // Create instance
    id obj = class_createInstance(cls, 0);

    // Cast to our dirty struct
    struct MyRawObject *hack = (struct MyRawObject *)obj;
    hack->x = 1337;
    hack->y = 4242;

    // Call method (normally!)
    objc_msgSend(obj, sel_getUid("doSomething"));

    // Cleanup
    object_dispose(obj);  // Normally handled by ARC or NSObject superclass
    return 0;
}
