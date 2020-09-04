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

        const mt::vec2f & size = m_surface->getSize();
        const mt::vec2f & offset = m_surface->getOffset();

        Polygon polygon;
        polygon.append( {offset.x + 0.f, offset.y + 0.f} );
        polygon.append( {offset.x + size.x, offset.y + 0.f} );
        polygon.append( {offset.x + size.x, offset.y + size.y} );
        polygon.append( {offset.x + 0.f, offset.y + size.y} );

        this->setPolygon( polygon );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotSurface::_release()
    {
        m_surface->release();
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
}
