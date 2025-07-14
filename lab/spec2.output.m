==== impl ====
#import <objc/runtime.h>
#import <objc/objc.h>
#import <objc/message.h>
#include <stdio.h>
#include <stdlib.h>
/****    IFACE START    *****/

struct Name {
 /*todo: reserve bytes for parents?*/
Class isa ;
int ivar ;

}; Class Name;
void __ObjcGenerated_helloWith(struct Name * self, SEL _cmd, int a);
id __ObjcGenerated_alloc(struct Name * self, SEL _cmd);
id __ObjcGenerated_initWithValueand(SEL _cmd, int value, int B);
void __ObjcGenerated_printIvar(SEL _cmd);
void __ObjcGenerated_dealloc(SEL _cmd);

/****    IFACE END    *****/

/****    IMPL START    *****/
void __ObjcGenerated_helloWith(struct Name * self, SEL _cmd, int a){
 printf( "helloWith called with a = %d\n" , a ) ;
}
id __ObjcGenerated_alloc(struct Name * self, SEL _cmd){
return class_createInstance(self, 0);}
id __ObjcGenerated_initWithValue(SEL _cmd, int value){
 self->ivar=value ;
return self ;
}
void __ObjcGenerated_printIvar(SEL _cmd){
 printf( "Current ivar value = %d\n" , self->ivar ) ;
}
void __ObjcGenerated_dealloc(SEL _cmd){
 printf( "dealloc called, ivar = %d\n" , ivar ) ;
 object_dispose( self ) ;
}

/****    IMPL END    *****/
int main(int argc, const char * argv[]){
 objc_msgSend( Name , "helloWith:",  7 ) ;
SEL test=sel_getUid("alloc") ;
Name *defaultObj=objc_msgSend( objc_msgSend( Name , "test") , "initWithValue:and:",  123 ,  8+8 ) ;
 objc_msgSend( defaultObj , "printIvar") ;
 objc_msgSend( defaultObj , "dealloc") ;
return 0;}

void __init() {

}
==== init ====
----------
