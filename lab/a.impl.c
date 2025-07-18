#include "a.iface.h"
#include "a.lib.h"
#include <stdio.h>
#include <stdlib.h>
/****    IMPL START    *****/
Root* __Root_new(Root * self){
return class_createInstance(self, 0);}
void __Root_delete(Root * self){
 object_dispose((id) self);
}
void __Root_sayHello(Root * self){
 printf("Hello from Root!\n");
}

struct Root_vtable RootClassVtable = {
    .size = sizeof(Root),
    .name = "Root",
    .super = NULL,
    .new = (void*)&__Root_new,
    .delete = (void*)&__Root_delete,
    .sayHello = (void*)&__Root_sayHello,
};
Root RootClassImpl = { .vtable = &RootClassVtable };
Root *RootClass = &RootClassImpl;

/****    IMPL END    *****/

/****    IMPL START    *****/
Name* __Name_new(Name * self){
return self->vtable->super->new(self);
}
Name* __Name_initWithValueand(Name * self, int value, int B){
 self-> ivar= value;
return self;
}
void __Name_printIvar(Name * self){
 printf("Current ivar value = %d\n",  self-> ivar);
}
void __Name_sayHello(Name * self){
 printf("Hello from Name! ivar = %d\n",  self-> ivar);
}

struct Name_vtable NameClassVtable = {
    .size = sizeof(Name),
    .name = "Name",
    .super = &RootClassVtable,
    .new = (void*)&__Name_new,
    .delete = (void*)&__Root_delete,
    .sayHello = (void*)&__Name_sayHello,
    .initWithValueand = (void*)&__Name_initWithValueand,
    .printIvar = (void*)&__Name_printIvar,
};
Name NameClassImpl = { .vtable = &NameClassVtable };
Name *NameClass = &NameClassImpl;

/****    IMPL END    *****/

/****    IMPL START    *****/
void __OtherName_sayHello(OtherName * self){
 printf("Hello from OtherName!\n");
}

struct OtherName_vtable OtherNameClassVtable = {
    .size = sizeof(OtherName),
    .name = "OtherName",
    .super = &RootClassVtable,
    .new = (void*)&__Root_new,
    .delete = (void*)&__Root_delete,
    .sayHello = (void*)&__Root_sayHello,
};
OtherName OtherNameClassImpl = { .vtable = &OtherNameClassVtable };
OtherName *OtherNameClass = &OtherNameClassImpl;

/****    IMPL END    *****/
int main(int argc, const char * argv[]){
Root* obj1=  NameClass->vtable->new( NameClass);
[obj1 initWithValue:42 and:0];
    
    Root* obj2=  OtherNameClass->vtable->new( OtherNameClass);
  obj1->vtable->sayHello( obj1);
  obj2->vtable->sayHello( obj2);
  obj1->vtable->delete( obj1);
  obj2->vtable->delete( obj2);
return 0;}
