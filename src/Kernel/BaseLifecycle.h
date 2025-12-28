#pragma once

#include "Interface/LifecycleInterface.h"

namespace Mengine
{
    class BaseLifecycle
        : public LifecycleInterface
    {
    public:
        BaseLifecycle();
        ~BaseLifecycle() override;

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
