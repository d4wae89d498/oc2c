#import <objc/runtime.h>
#import <objc/objc.h>
#import <objc/message.h>
#include <stdio.h>
#include <stdlib.h>
/****    IMPL START    *****/
id __ObjcGenerated_alloc(Class self, SEL _cmd){
return class_createInstance(self, 0);}
id __ObjcGenerated_initWithValueand(struct Name * self, SEL _cmd, int value, int B){
 self-> ivar= value;
return self;
}
void __ObjcGenerated_printIvar(struct Name * self, SEL _cmd){
 printf("Current ivar value = %d\n",  self-> ivar);
}
void __ObjcGenerated_dealloc(struct Name * self, SEL _cmd){
 object_dispose( self);
}

/****    IMPL END    *****/
int main(int argc, const char * argv[]){
objc_msgSend( NameClass, "helloWith:", 8);
id defaultObj=objc_msgSend(objc_msgSend( NameClass, "alloc"), "initWithValue:and:", 123, 8+8);
objc_msgSend( defaultObj, "printIvar");
objc_msgSend( defaultObj, "dealloc");
return 0;}
