#pragma once

#include "Interface/DispatchableInterface.h"

namespace Mengine
{
    class DispatchableBase
        : public DispatchableInterface
    {
    public:
        DispatchableBase();
        ~DispatchableBase() override;

    private:
        void preUpdate() override;
        void update() override;
        void postUpdate() override;

    public:
        virtual void _preUpdate();
        virtual void _update();
        virtual void _postUpdate();
    };
}
