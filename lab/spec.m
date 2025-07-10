#import <stdio.h>
#import <stdlib.h>

#import <Foundation/Foundation.h> // required for NSObject


// Animal class with ivar and methods
@interface Animal : NSObject {
    const char *_name;
}
- (void)setName:(const char *)name;
- (const char *)name;

- (void)speak;
+ (const char *)species;
- (void)greet;
@end

@implementation Animal

- (void)someMethodWithA:(int)a b:(int)b  {
    NSLog(@"%d %d", a, b);
}


- (void)setName:(const char *)name {
    _name = name;
}

- (const char *)name {
    return _name;
}

- (void)speak {
    printf("%s makes a sound.\n", [self name]);
}

+ (const char *)species {
    return "Animalia";
}

- (void)greet {
    printf("%s greets in an animal way.\n", [self name]);
}

- (void)dealloc {
    [super dealloc];
}

@end

// Subclass Dog
@interface Dog : Animal {
    int _age;
}
- (void)setAge:(int)age;
- (int)age;

- (void)speak;
- (void)greet;
@end

@implementation Dog

- (void)setAge:(int)age {
    _age = age;
}

- (int)age {
    return _age;
}

- (void)speak {
    printf("%s barks.\n", [self name]);
}

- (void)greet {
    printf("%s wags its tail.\n", [self name]);
}

- (void)dealloc {
    [super dealloc];
}

@end

int main() {
    Dog *fido = [[Dog alloc] init];
    [fido setName:"Fido"];
    [fido setAge:5];

    [fido speak]; // overridden
    [fido greet]; // overridden

    printf("Species: %s\n", [Dog species]); // class method

    // Dynamic typing
    id obj = fido;
    SEL selector = @selector(speak);
    if ([obj respondsToSelector:selector]) {
        [obj performSelector:selector];
    }

    [fido release];
    return 0;
}
