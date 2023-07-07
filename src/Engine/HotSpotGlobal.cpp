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
    bool HotSpotGlobal::testPoint( const RenderContext * _context, const Resolution & _contentResolution, const mt::vec2f & _point ) const
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _contentResolution );
        MENGINE_UNUSED( _point );

        return !m_outward;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotGlobal::testRadius( const RenderContext * _context, const Resolution & _contentResolution, const mt::vec2f & _point, float _radius ) const
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _contentResolution );
        MENGINE_UNUSED( _point );
        MENGINE_UNUSED( _radius );

        return !m_outward;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotGlobal::testPolygon( const RenderContext * _context, const Resolution & _contentResolution, const mt::vec2f & _point, const Polygon & _polygon ) const
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _contentResolution );
        MENGINE_UNUSED( _point );
        MENGINE_UNUSED( _polygon );

        return !m_outward;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotGlobal::testBounds( const RenderContext * _context, const Resolution & _contentResolution, float _left, float _right, float _top, float _bottom ) const
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _contentResolution );
        MENGINE_UNUSED( _left );
        MENGINE_UNUSED( _right );
        MENGINE_UNUSED( _top );
        MENGINE_UNUSED( _bottom );

        return !m_outward;
    }
    //////////////////////////////////////////////////////////////////////////
}
