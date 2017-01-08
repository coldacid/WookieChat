#include <stdio.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <stdarg.h>


#ifdef __cplusplus
extern "C" {
#endif


Object * VARARGS68K DoSuperNew(struct IClass *cl, Object *obj, ...)
{
Object *rc;
va_list args;

va_startlinear(args, obj);
rc = (Object *)DoSuperMethod(cl, obj, OM_NEW, va_getlinearva(args, ULONG), NULL);
va_end(args);

return rc;
} 

#ifdef __cplusplus
}
#endif

