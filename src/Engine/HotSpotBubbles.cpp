#include "HotSpotBubbles.h"

#include "Interface/RenderCameraInterface.h"
#include "Interface/EnumeratorServiceInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    HotSpotBubbles::HotSpotBubbles()
        : m_invalidateBubbleWM( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    HotSpotBubbles::~HotSpotBubbles()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t HotSpotBubbles::addBubble( const mt::vec2f & _pos, float _radius, float _ellipse, bool _outward )
    {
        Bubble bubble;

        UniqueId new_id = GENERATE_UNIQUE_IDENTITY();

        bubble.id = new_id;
        bubble.pos = _pos;
        bubble.pos_wm = _pos;
        bubble.radius = _radius;
        bubble.ellipse = _ellipse;
        bubble.outward = _outward;

        m_bubbles.emplace_back( bubble );

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotBubbles::testPoint( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point ) const
    {
        MENGINE_UNUSED( _contentResolution );
        MENGINE_UNUSED( _viewport );

        if( m_global == true )
        {
            return !m_outward;
        }

        const mt::mat4f & vm_inv = _camera->getCameraViewMatrixInv();

        mt::vec2f pointIn1;
        mt::mul_v2_v2_m4( pointIn1, _point, vm_inv );

        this->updateBubbleWM_();

        for( const Bubble & bubble : m_bubbles )
        {
            mt::vec2f v = pointIn1 - bubble.pos_wm;

            v.y /= bubble.ellipse;

            float v_sqrlength = v.sqrlength();

            if( v_sqrlength < bubble.radius * bubble.radius )
            {
                return !m_outward;
            }
        }

        return m_outward;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotBubbles::testRadius( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, float _radiusx, float _radiusy ) const
    {
        MENGINE_UNUSED( _contentResolution );
        MENGINE_UNUSED( _viewport );

        if( m_global == true )
        {
            return !m_outward;
        }

        const mt::mat4f & vm_inv = _camera->getCameraViewMatrixInv();

        mt::vec2f pointIn1;
        mt::mul_v2_v2_m4( pointIn1, _point, vm_inv );

        this->updateBubbleWM_();

        for( const Bubble & b : m_bubbles )
        {
            mt::vec2f v = pointIn1 - b.pos_wm;

            v.y /= b.ellipse;

            float v_sqrlength = v.sqrlength();

            if( v_sqrlength < (b.radius + _radiusx) * (b.radius + _radiusy) )
            {
                return !m_outward;
            }
        }

        return m_outward;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotBubbles::testPolygon( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, const Polygon & _polygon ) const
    {
        if( m_global == true )
        {
            return !m_outward;
        }

        //TODO:
        MENGINE_UNUSED( _polygon );

        bool test = this->testPoint( _camera, _viewport, _contentResolution, _point );

        return test;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotBubbles::_invalidateWorldMatrix() const
    {
        HotSpot::_invalidateWorldMatrix();

        this->invalidatemBubbleWM_();
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotBubbles::invalidatemBubbleWM_() const
    {
        m_invalidateBubbleWM = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotBubbles::updateBubbleWM_() const
    {
        if( m_invalidateBubbleWM == false )
        {
            return;
        }

        m_invalidateBubbleWM = false;

        const mt::mat4f & wm = this->getWorldMatrix();

        for( const Bubble & b : m_bubbles )
        {
            mt::mul_v2_v2_m4( b.pos_wm, b.pos, wm );
        }
    }

}
