#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "objc/runtime.h"

int main() {
    SEL sel1 = sel_registerName("foo");
    SEL sel2 = sel_registerName("foo");
    SEL sel3 = sel_registerName("bar");
    
    assert(sel1 != NULL);
    assert(sel2 != NULL);
    assert(sel3 != NULL);
    assert(sel1 == sel2);  // Same name should return same SEL
    assert(sel1 != sel3);  // Different names should return different SELs
    
    assert(strcmp(sel_getName(sel1), "foo") == 0);
    assert(strcmp(sel_getName(sel3), "bar") == 0);
    
    printf("sel1 == sel2: %s\n", (sel1 == sel2) ? "yes" : "no");
    printf("sel1 == sel3: %s\n", (sel1 == sel3) ? "yes" : "no");
    printf("sel_getName(sel1): %s\n", sel_getName(sel1));
    printf("sel_getName(sel3): %s\n", sel_getName(sel3));
    
    printf("test_selector_uniqueness: PASSED\n");
    return 0;
} 