#import <objc/runtime.h>
#import <objc/objc.h>
#import <objc/message.h>
#include <stdio.h>
#include <stdlib.h>
/****    IFACE START    *****/

typedef struct Name {
 /*todo: reserve bytes for parents?*/
Class isa;
int ivar;

} Name; Class NameClass;
id __ObjcGenerated_alloc(struct Name * self, SEL _cmd);
id __ObjcGenerated_initWithValueand(SEL _cmd, int value, int B);
void __ObjcGenerated_printIvar(SEL _cmd);
void __ObjcGenerated_dealloc(SEL _cmd);

/****    IFACE END    *****/

/****    IMPL START    *****/
id __ObjcGenerated_alloc(struct Name * self, SEL _cmd){
return class_createInstance(self, 0);}
id __ObjcGenerated_initWithValueand(SEL _cmd, int value, int B){
 self-> ivar= value;
return self;
}
void __ObjcGenerated_printIvar(SEL _cmd){
 printf("Current ivar value = %d\n",  self-> ivar);
}
void __ObjcGenerated_dealloc(SEL _cmd){
 printf("dealloc called, ivar = %d\n",  ivar);
 object_dispose( self);
}

/****    IMPL END    *****/
int main(int argc, const char * argv[]){
objc_msgSend( NameClass, "helloWith:", 8);
id defaultObj=objc_msgSend(objc_msgSend( NameClass, "alloc"), "initWithValue:and:", 123, 8+8);
objc_msgSend( defaultObj, "printIvar");
objc_msgSend( defaultObj, "dealloc");
return 0;}

void __init() {
NameClass = objc_allocateClassPair(NULL, "Name", sizeof(Name) - sizeof(void *));
}

