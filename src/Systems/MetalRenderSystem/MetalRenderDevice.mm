#include "MetalRenderDevice.h"

#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetalRenderDevice::MetalRenderDevice()
        : m_device( nil )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetalRenderDevice::~MetalRenderDevice()
    {
        MENGINE_ASSERTION_FATAL( m_device == nil, "device is not released" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderDevice::initialize( id<MTLDevice> _device )
    {
        if( _device == nil )
        {
            return false;
        }

        m_device = _device;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderDevice::finalize()
    {
        m_device = nil;
    }
    //////////////////////////////////////////////////////////////////////////
    id<MTLDevice> MetalRenderDevice::getMetalDevice() const
    {
        return m_device;
    }
    //////////////////////////////////////////////////////////////////////////
}
