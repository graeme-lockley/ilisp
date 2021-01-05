#include "value.h"

struct ValueStruct *VNil = {VP_IMMUTABLE | VP_PINNED | VT_NIL << 2};

struct ValueStruct *mkNil()
{
    return VNil;
}
