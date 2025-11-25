#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#define DEBUG_ASSERT(expression) \
if(!expression) {  \
printf("[Debug Assert #1] (%s)\n -- %s:%d: %s\n", __FILE__, __FUNCTION__, __LINE__, #expression);\
exit(0);\
}

#define REF(type) const type&

#endif