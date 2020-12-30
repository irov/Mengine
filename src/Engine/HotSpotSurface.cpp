#include "HotSpotSurface.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    HotSpotSurface::HotSpotSurface()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    HotSpotSurface::~HotSpotSurface()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotSurface::setSurface( const SurfacePtr & _surface )
    {
        if( m_surface == _surface )
        {
            return;
        }

        this->recompile( [this, &_surface]()
        {
            m_surface = _surface;

            if( m_surface == nullptr )
            {
                return false;
            }

            return true;
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    const SurfacePtr & HotSpotSurface::getSurface() const
    {
        return m_surface;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotSurface::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_surface, "'%s' surface is nullptr"
            , this->getName().c_str()
        );

        if( m_surface->compile() == false )
        {
            LOGGER_ERROR( "'%s' surface '%s' not compile"
                , this->getName().c_str()
                , m_surface->getName().c_str()
            );

            return false;
        }

        const mt::vec2f & maxSize = m_surface->getMaxSize();
        const mt::vec2f & size = m_surface->getSize();
        const mt::vec2f & offset = m_surface->getOffset();
        const mt::vec2f & anchor = m_surface->getAnchor();

        mt::vec2f total_offset = offset - anchor * maxSize;

        Polygon polygon;
        polygon.append( {total_offset.x + 0.f, total_offset.y + 0.f} );
        polygon.append( {total_offset.x + size.x, total_offset.y + 0.f} );
        polygon.append( {total_offset.x + size.x, total_offset.y + size.y} );
        polygon.append( {total_offset.x + 0.f, total_offset.y + size.y} );

        this->setPolygon( polygon );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotSurface::_release()
    {
        m_surface->release();
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotSurface::_dispose()
    {
        m_surface = nullptr;

        HotSpotPolygon::_dispose();
    }    
    //////////////////////////////////////////////////////////////////////////
}
