#pragma once

#include "Config/Config.h"

#ifndef MENGINE_MESSAGE_CASE
#define MENGINE_MESSAGE_CASE(id, msg)\
        case id:\
             return msg " [" #id "]"
#endif