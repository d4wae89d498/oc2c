#import <objc/runtime.h>
#import <objc/objc.h>
#import <objc/message.h>
#include <stdio.h>
#include <stdlib.h>


@interface Name {
    Class isa;
    int ivar;
}

+ (void)helloWith:(int)a;
+ (id)alloc;

- (id)initWithValue:(int)value and:(int)B;

- (void)printIvar;
- (void)dealloc;

@end

@implementation Name

+ (void)helloWith:(int)a {
    printf("helloWith called with a = %d\n", a);
}

+ (id)alloc {
    return class_createInstance(self, 0);
}

- (id)initWithValue:(int)value {
    ivar = value;
    return self;
}

- (void)printIvar {
    printf("Current ivar value = %d\n", self->ivar);
}

- (void)dealloc {
    printf("dealloc called, ivar = %d\n", ivar);
  //  free(self);
    object_dispose(self);
  //  [object_getClass(self) dealloc];
}

@end

int main(int argc, const char * argv[]) {

    [Name helloWith:7];

    Name *defaultObj = [[Name alloc] initWithValue:123];

    [defaultObj printIvar]; 


    objc_msgSend(defaultObj, @selector(printIvar:));

    [defaultObj dealloc];


    return 0;
}
