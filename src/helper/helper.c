#include "helper.h"
#include <stdlib.h>

void freep(void *ptr)
{
    void **ptrptr = (void **)ptr;
    free(*ptrptr);
    *ptrptr = NULL;
}
