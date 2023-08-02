#pragma once

#include "Config/Config.h"

extern "C"
{
#define XMD_H
#undef FAR

#include <csetjmp>
#include "jinclude.h"
#include "jpeglib.h"
#include "jerror.h"
#include "jmemsys.h"
#define JPEG_INTERNALS
#include "jmorecfg.h"
}