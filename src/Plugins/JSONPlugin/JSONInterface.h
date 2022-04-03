#pragma once

#include "Interface/UnknownInterface.h"
#include "Interface/ServiceInterface.h"
#include "Interface/InputStreamInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/JSON.h"

namespace Mengine
{
    class UnknownResourceJSONInterface
        : public UnknownInterface
    {
    public:
        virtual const jpp::object & getJSON() const = 0;
    };
}