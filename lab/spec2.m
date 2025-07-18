#import <objc/runtime.h>
#import <objc/objc.h>
#import <objc/message.h>
#include <stdio.h>
#include <stdlib.h>

@interface Root
+ (instancetype) new;
- (void) delete;
@end

@implementation Root 
+ (instancetype) new {
    return class_createInstance(self, 0);

}
- (void) delete {
    object_dispose((id)self);
}
@end

@interface Name : Root {
    int ivar;
}
+ (instancetype) new;
- (Name*)initWithValue:(int)value and:(int)B;
- (void)printIvar;
- (void) delete;
@end

@implementation Name 

+ (instancetype) new {
    return [super new];
}

- (void) delete {
    
}

- (Name*)initWithValue:(int)value and:(int)B {
    self->ivar = value;
    return self;
}

- (void)printIvar {
    printf("Current ivar value = %d\n", self->ivar);
}

@end

int main(int argc, const char * argv[]) {
    //[Name helloWith:8];
    Name* defaultObj = [Name new];// initWithValue:123 and:8+8];
    [defaultObj printIvar]; 
    [defaultObj delete];
    return 0;
}
