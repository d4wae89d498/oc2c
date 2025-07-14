#import <objc/runtime.h>
#import <objc/objc.h>
#import <objc/message.h>
#include <stdio.h>
#include <stdlib.h>


@interface Name {
    Class isa;
    int ivar;
}

+ (id)alloc;
- (id)initWithValue:(int)value and:(int)B;
- (void)printIvar;
- (void)dealloc;

@end

@implementation Name

+ (id)alloc {
    return class_createInstance(self, 0);
}

- (id)initWithValue:(int)value and:(int)B {
    self->ivar = value;
    return self;
}

- (void)printIvar {
    printf("Current ivar value = %d\n", self->ivar);
}

- (void)dealloc {
    printf("dealloc called, ivar = %d\n", ivar);
    object_dispose(self);
}

@end

int main(int argc, const char * argv[]) {
    [Name helloWith:8];
    id defaultObj = [[Name alloc] initWithValue:123 and:8+8];
    [defaultObj printIvar]; 
    [defaultObj dealloc];
    return 0;
}
