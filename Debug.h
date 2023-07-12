#pragma once

#ifdef DEBUG_SERIAL_OUT
#include "SerialOut.h"
namespace embedded
{
extern SerialOut SerialDebug;
}
#define DEBUG_LOG(x) { embedded::SerialDebug << x << embedded::SerialOut::endl{}; }
#else
#define DEBUG_LOG(x)
#endif
