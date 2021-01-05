#include "value.h"

static struct ValueStruct VNil_Value = {3, {0}};

struct ValueStruct *VNil = &VNil_Value;

struct ValueStruct *mkNil()
{
    return VNil;
}
