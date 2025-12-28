#pragma once

#include "Interface/DispatchableInterface.h"

namespace Mengine
{
    class BaseDispatchable
        : public DispatchableInterface
    {
    public:
        BaseDispatchable();
        ~BaseDispatchable() override;

    private:
        void preUpdate() override;
        void update() override;
        void postUpdate() override;

    protected:
        virtual void _preUpdate();
        virtual void _update();
        virtual void _postUpdate();
    };
}
