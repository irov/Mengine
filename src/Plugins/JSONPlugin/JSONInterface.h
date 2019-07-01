#pragma once

#include "Interface/UnknownInterface.h"

#include "jpp/jpp.hpp"

namespace Mengine
{
    class UnknownResourceJSONInterface
        : public UnknownInterface
    {
    public:
        virtual const jpp::object& getJSON() const = 0;
    };
}