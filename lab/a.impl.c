#include "a.iface.h"
#import <objc/runtime.h>
#import <objc/objc.h>
#import <objc/message.h>
#include <stdio.h>
#include <stdlib.h>
Class NameClass;

/****    IMPL START    *****/
id __ObjcGenerated_Name_alloc(Class self, SEL _cmd){
 printf("ALLOC!\n");
return class_createInstance(self, 0);}
id __ObjcGenerated_Name_initWithValueand(struct Name * self, SEL _cmd, int value, int B){
 self-> ivar= value;
return self;
}
void __ObjcGenerated_Name_printIvar(struct Name * self, SEL _cmd){
 printf("Current ivar value = %d\n",  self-> ivar);
}
void __ObjcGenerated_Name_dealloc(struct Name * self, SEL _cmd){
 object_dispose( self);
}

/****    IMPL END    *****/
int main(int argc, const char * argv[]){
//objc_msgSend( NameClass, sel_getUid("helloWith:"), 8);
id defaultObj=objc_msgSend(objc_msgSend( NameClass, sel_getUid("alloc")), sel_getUid("initWithValue:and:"), 123, 8+8);
objc_msgSend( defaultObj, sel_getUid("printIvar"));
objc_msgSend( defaultObj, sel_getUid("dealloc"));
return 0;}
