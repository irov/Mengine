#pragma once

#include "Kernel/Mixin.h"

namespace Mengine
{
    class Content;

    class Contentable
        : public Mixin
    {
    public:
        virtual Content * getContent()
        {
            return nullptr;
        };
    };
}

#define DECLARE_CONTENTABLE()\
public:\
    Mengine::Content * getContent() override{ return this; }\
protected:
