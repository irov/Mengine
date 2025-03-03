#include "HotSpotImage.h"

#include "Interface/ResourceServiceInterface.h"

#include "Kernel/RenderCameraHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    HotSpotImage::HotSpotImage()
        : m_alphaTest( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    HotSpotImage::~HotSpotImage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotImage::setResourceTestPick( const ResourceTestPickPtr & _resourceTestPick )
    {
        if( m_resourceTestPick == _resourceTestPick )
        {
            return;
        }

        this->recompile( [this, &_resourceTestPick]()
        {
            m_resourceTestPick = _resourceTestPick;

            if( m_resourceTestPick == nullptr )
            {
                return false;
            }

            return true;
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceTestPickPtr & HotSpotImage::getResourceTestPick() const
    {
        return m_resourceTestPick;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotImage::setAlphaTest( float _value )
    {
        m_alphaTest = _value;
    }
    //////////////////////////////////////////////////////////////////////////
    float HotSpotImage::getAlphaTest() const
    {
        return m_alphaTest;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t HotSpotImage::getWidth() const
    {
        if( m_resourceTestPick == nullptr )
        {
            LOGGER_ERROR( "hotspot image '%s' not compiled"
                , this->getName().c_str()
            );

            return 0;
        }

        uint32_t width = m_resourceTestPick->getImageWidth();

        return width;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t HotSpotImage::getHeight() const
    {
        if( m_resourceTestPick == nullptr )
        {
            LOGGER_ERROR( "hotspot image '%s' not compiled"
                , this->getName().c_str()
            );

            return 0;
        }

        uint32_t height = m_resourceTestPick->getImageHeight();

        return height;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotImage::_compile()
    {
        if( HotSpot::_compile() == false )
        {
            return false;
        }

        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceTestPick, "'%s' resource is nullptr"
            , this->getName().c_str()
        );

        if( m_resourceTestPick->compile() == false )
        {
            LOGGER_ERROR( "hotspot image '%s' can't compile HIT resource '%s'"
                , this->getName().c_str()
                , m_resourceTestPick->getName().c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotImage::_release()
    {
        m_resourceTestPick->release();

        HotSpot::_release();
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotImage::_dispose()
    {
        m_resourceTestPick = nullptr;

        HotSpot::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotImage::testPoint( const RenderContext * _context, const mt::vec2f & _point ) const
    {
        const RenderResolutionInterface * resolution = _context->resolution;
        const RenderCameraInterface * camera = _context->camera;
        const RenderViewportInterface * viewport = _context->viewport;

        if( m_global == true )
        {
            return !m_outward;
        }

        mt::box2f bb_screen;
        this->getScreenBoundingBox( _context, &bb_screen );

        if( mt::box2_intersect( bb_screen, _point ) == false )
        {
            return m_outward;
        }

        const mt::mat4f & vpm_inv = camera->getCameraViewProjectionMatrixInv();
        const Viewport & vp = viewport->getViewportWM();
        

        mt::vec2f point_vp;
        resolution->fromScreenToContentPosition( _point, &point_vp );

        point_vp -= vp.begin;

        mt::vec2f size = vp.size();

        if( size.x < mt::constant::eps || size.y < mt::constant::eps )
        {
            return m_outward;
        }

        point_vp /= size;

        mt::vec2f point_norm;
        point_norm.x = point_vp.x * 2.f - 1.f;
        point_norm.y = 1.f - point_vp.y * 2.f;

        mt::vec2f pointIn1;
        mt::mul_v2_v2_m4( &pointIn1, point_norm, vpm_inv );

        const mt::mat4f & wm = this->getWorldMatrix();

        mt::mat4f invWM;
        mt::inv_m4_m4( &invWM, wm );

        mt::vec2f pointIn2;
        mt::mul_v2_v2_m4( &pointIn2, pointIn1, invWM );

        bool result = m_resourceTestPick->testPoint( pointIn2, m_alphaTest );

        return result != m_outward;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotImage::testRadius( const RenderContext * _context, const mt::vec2f & _point, float _radius ) const
    {
        const RenderResolutionInterface * resolution = _context->resolution;
        const RenderCameraInterface * camera = _context->camera;
        const RenderViewportInterface * viewport = _context->viewport;

        if( m_global == true )
        {
            return !m_outward;
        }

        mt::vec2f rxy;
        Helper::worldToScreenDelta( _context, mt::vec2f( _radius, _radius ), &rxy );

        mt::box2f bb_screen;
        this->getScreenBoundingBox( _context, &bb_screen );

        if( mt::box2_intersect( bb_screen, _point, rxy.x, rxy.y ) == false )
        {
            return m_outward;
        }

        const mt::mat4f & vpm_inv = camera->getCameraViewProjectionMatrixInv();
        const Viewport & vp = viewport->getViewportWM();

        mt::vec2f point_vp;
        resolution->fromScreenToContentPosition( _point, &point_vp );

        point_vp -= vp.begin;

        mt::vec2f size = vp.size();

        if( size.x < mt::constant::eps || size.y < mt::constant::eps )
        {
            return m_outward;
        }

        point_vp /= size;

        mt::vec2f point_norm;
        point_norm.x = point_vp.x * 2.f - 1.f;
        point_norm.y = 1.f - point_vp.y * 2.f;

        mt::vec2f pointIn1;
        mt::mul_v2_v2_m4( &pointIn1, point_norm, vpm_inv );

        const mt::mat4f & wm = this->getWorldMatrix();

        mt::mat4f invWM;
        mt::inv_m4_m4( &invWM, wm );

        mt::vec2f pointIn2;
        mt::mul_v2_v2_m4( &pointIn2, pointIn1, invWM );

        bool result = m_resourceTestPick->testRadius( pointIn2, _radius, m_alphaTest );

        return result != m_outward;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotImage::testPolygon( const RenderContext * _context, const mt::vec2f & _point, const Polygon & _polygon ) const
    {
        if( m_global == true )
        {
            return !m_outward;
        }

        if( _polygon.empty() == true )
        {
            return m_outward;
        }

        mt::box2f bb_screen;
        this->getScreenBoundingBox( _context, &bb_screen );

        mt::box2f bb_polygon;
        _polygon.to_box2f( &bb_polygon );

        mt::box2f bb_polygon_screen;
        Helper::worldToScreenBox( _context, bb_polygon, &bb_polygon_screen );

        mt::box2_transpose( &bb_polygon_screen, _point );

        if( mt::box2_intersect( bb_screen, bb_polygon_screen ) == false )
        {
            return m_outward;
        }

        return !m_outward;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotImage::testBounds( const RenderContext * _context, float _left, float _right, float _top, float _bottom ) const
    {
        if( m_global == true )
        {
            return !m_outward;
        }

        mt::box2f bb;
        this->getScreenBoundingBox( _context, &bb );

        if( bb.minimum.x < _left )
        {
            return m_outward;
        }

        if( bb.maximum.x > _right )
        {
            return m_outward;
        }

        if( bb.minimum.y < _top )
        {
            return m_outward;
        }

        if( bb.maximum.y > _bottom )
        {
            return m_outward;
        }

        return !m_outward;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotImage::getWorldBoundingBox( mt::box2f * const _bb ) const
    {
        const mt::mat4f & wm = this->getWorldMatrix();

        float hs_width = m_resourceTestPick->getImageWidthF();
        float hs_height = m_resourceTestPick->getImageHeightF();

        mt::vec2f minimal( 0.f, 0.f );
        mt::vec2f maximal( hs_width, hs_height );

        mt::vec2f minimal_wm;
        mt::mul_v2_v2_m4( &minimal_wm, minimal, wm );

        mt::vec2f maximal_wm;
        mt::mul_v2_v2_m4( &maximal_wm, maximal, wm );

        mt::box2f bb;
        mt::box2_set_from_two_point( &bb, minimal_wm, maximal_wm );

        *_bb = bb;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotImage::getScreenBoundingBox( const RenderContext * _context, mt::box2f * const _bb ) const
    {
        mt::box2f bb;
        this->getWorldBoundingBox( &bb );

        mt::box2f bb_screen;
        Helper::worldToScreenBox( _context, bb, &bb_screen );

        *_bb = bb_screen;
    }
    //////////////////////////////////////////////////////////////////////////
}
