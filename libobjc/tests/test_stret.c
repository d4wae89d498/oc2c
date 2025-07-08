#include <stdio.h>
#include "objc/message.h"

typedef struct {
    int x, y;
} Point;

void point_method(Point *p, id self, SEL _cmd, ...) {
    *p = (Point){42, 84};
    printf("point_method: returning Point{%d, %d}\n", p->x, p->y);
    return;
}

int main() {
    SEL sel_point = sel_registerName("point");
    Class MyClass = objc_allocateClassPair("StretClass", Object_class, 0);
    class_addMethod(MyClass, sel_point, (IMP)point_method, "{Point=ii}@:");
    objc_registerClassPair(MyClass);
    id obj = objc_alloc(MyClass);
    objc_init(obj);
    Point result;
    objc_msgSend_stret(&result, obj, sel_point);
    printf("Received: Point{%d, %d}\n", result.x, result.y);
    objc_dealloc(obj);
    return 0;
} 