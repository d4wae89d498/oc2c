==== impl ====
#import <objc/runtime.h>
#import <objc/objc.h>
#import <objc/message.h>
#include <stdio.h>
#include <stdlib.h>
/****    IFACE START    *****/

struct __ObjcGenerated_Name {
 /*todo: reserve bytes for parents?*/
Class isa ;
int ivar ;

};
void __ObjcGenerated_helloWith(SEL _cmd, int a);
id __ObjcGenerated_alloc(SEL _cmd);
id __ObjcGenerated_initWithValueand(struct Name * self, SEL _cmd, int value, int B);
void __ObjcGenerated_printIvar(struct Name * self, SEL _cmd);
void __ObjcGenerated_dealloc(struct Name * self, SEL _cmd);

/****    IFACE END    *****/

/****    IMPL START    *****/
void __ObjcGenerated_helloWith(SEL _cmd, int a){
 printf( "helloWith called with a = %d\n" , a ) ;
}
id __ObjcGenerated_alloc(SEL _cmd){
return class_createInstance(self, 0);}
id __ObjcGenerated_initWithValue(struct Name * self, SEL _cmd, int value){
 ivar=value ;
return self ;
}
void __ObjcGenerated_printIvar(struct Name * self, SEL _cmd){
 printf( "Current ivar value = %d\n" , self->ivar ) ;
}
void __ObjcGenerated_dealloc(struct Name * self, SEL _cmd){
 printf( "dealloc called, ivar = %d\n" , ivar ) ;
 object_dispose( self ) ;
}

/****    IMPL END    *****/
int main(int argc, const char * argv[]){
 objc_msgSend( Name , helloWith: 7 ) ;
Name *defaultObj=objc_msgSend( objc_msgSend( Name , alloc) , initWithValue: 123 ) ;
 objc_msgSend( defaultObj , printIvar) ;
 objc_msgSend( defaultObj , sel_getUid("printIvar:") ) ;
 objc_msgSend( defaultObj , dealloc) ;
return 0;}

void __init() {

}
==== init ====
----------
