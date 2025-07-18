#include "a.lib.h"
#include <stdio.h>
#include <stdlib.h>

@interface Root
+ (instancetype)new;
- (void)delete;
- (void)sayHello;
@end

@implementation Root
+ (instancetype)new {
    return class_createInstance(self, 0);
}
- (void)delete {
    object_dispose((id)self);
}
- (void)sayHello {
    printf("Hello from Root!\n");
}
@end

@interface Name : Root {
    int ivar;
}
+ (instancetype)new;
- (Name*)initWithValue:(int)value and:(int)B;
- (void)printIvar;
- (void)sayHello;  // override
@end

@implementation Name
+ (instancetype)new {
    return [super new];
}
- (Name*)initWithValue:(int)value and:(int)B {
    self->ivar = value;
    return self;
}
- (void)printIvar {
    printf("Current ivar value = %d\n", self->ivar);
}
- (void)sayHello {
    printf("Hello from Name! ivar = %d\n", self->ivar);
}
@end

@interface OtherName : Root
@end

@implementation OtherName
- (void)sayHello {
    printf("Hello from OtherName!\n");
}
@end

int main(int argc, const char * argv[]) {
    Root *obj1 = [Name new];
    [obj1 initWithValue:42 and:0];
    
    Root *obj2 = [OtherName new];
    
    [obj1 sayHello];   // prints Name’s version
    [obj2 sayHello];   // prints OtherName’s version
    
    [obj1 delete];
    [obj2 delete];
    return 0;
}
