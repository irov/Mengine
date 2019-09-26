#pragma once

#include "Kernel/Mixin.h"

namespace Mengine
{
    class ContentInterface;

    class Contentable
        : public Mixin
    {
    public:
        virtual ContentInterface * getContent()
        {
            return nullptr;
        };

        virtual const ContentInterface * getContent() const
        {
            return nullptr;
        };
    };
}

#define DECLARE_CONTENTABLE()\
public:\
    Mengine::ContentInterface * getContent() override{ return this; }\
    const Mengine::ContentInterface * getContent() const override{ return this; }\
protected:
