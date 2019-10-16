#include "HotSpotGlobal.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/RenderCameraInterface.h"
#include "Interface/RenderViewportInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    HotSpotGlobal::HotSpotGlobal()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    HotSpotGlobal::~HotSpotGlobal()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotGlobal::testPoint( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point ) const
    {
        MENGINE_UNUSED( _camera );
        MENGINE_UNUSED( _viewport );
        MENGINE_UNUSED( _contentResolution );
        MENGINE_UNUSED( _point );

        return !m_outward;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotGlobal::testRadius( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, float _radiusx, float _radiusy ) const
    {
        MENGINE_UNUSED( _camera );
        MENGINE_UNUSED( _viewport );
        MENGINE_UNUSED( _contentResolution );
        MENGINE_UNUSED( _point );
        MENGINE_UNUSED( _radiusx );
        MENGINE_UNUSED( _radiusy );

        return !m_outward;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotGlobal::testPolygon( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, const Polygon & _polygon ) const
    {
        MENGINE_UNUSED( _camera );
        MENGINE_UNUSED( _viewport );
        MENGINE_UNUSED( _contentResolution );
        MENGINE_UNUSED( _point );
        MENGINE_UNUSED( _polygon );

        return !m_outward;
    }
    //////////////////////////////////////////////////////////////////////////
}
