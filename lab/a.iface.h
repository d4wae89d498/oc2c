#ifndef ___a_h
# define ___a_h
# include <objc/runtime.h>
# include <objc/message.h>

/****    IFACE START    *****/

typedef struct Name {
 /*todo: reserve bytes for parents?*/
Class isa;
int ivar;

} Name; extern Class NameClass;
id __ObjcGenerated_Name_alloc(Class self, SEL _cmd);
id __ObjcGenerated_Name_initWithValueand(struct Name * self, SEL _cmd, int value, int B);
void __ObjcGenerated_Name_printIvar(struct Name * self, SEL _cmd);
void __ObjcGenerated_Name_dealloc(struct Name * self, SEL _cmd);

/****    IFACE END    *****/
#endif
