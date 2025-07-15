#include "a.iface.h"

void __init() {
NameClass = objc_allocateClassPair(NULL, "Name", sizeof(Name) - sizeof(void *));
objc_registerClassPair(NameClass);
class_addMethod(NameClass, sel_registerName("alloc"), (IMP)__ObjcGenerated_Name_alloc, "");
class_addMethod(NameClass, sel_registerName("initWithValue:and:"), (IMP)__ObjcGenerated_Name_initWithValueand, "");
class_addMethod(NameClass, sel_registerName("printIvar"), (IMP)__ObjcGenerated_Name_printIvar, "");
class_addMethod(NameClass, sel_registerName("dealloc"), (IMP)__ObjcGenerated_Name_dealloc, "");
}
