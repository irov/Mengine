#pragma once

#include "MockupRenderTargetTexture.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MockupRenderTargetOffscreen
        : public MockupRenderTargetTexture
    {
    public:
        MockupRenderTargetOffscreen();
        ~MockupRenderTargetOffscreen() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool getData( void * const _buffer, size_t _pitch ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MockupRenderTargetOffscreen> MockupRenderTargetOffscreenPtr;
    //////////////////////////////////////////////////////////////////////////
}