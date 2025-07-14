
/****    IFACE START    *****/

typedef struct Name {
 /*todo: reserve bytes for parents?*/
Class isa;
int ivar;

} Name; Class NameClass;
id __ObjcGenerated_alloc(Class self, SEL _cmd);
id __ObjcGenerated_initWithValueand(struct Name * self, SEL _cmd, int value, int B);
void __ObjcGenerated_printIvar(struct Name * self, SEL _cmd);
void __ObjcGenerated_dealloc(struct Name * self, SEL _cmd);

/****    IFACE END    *****/
