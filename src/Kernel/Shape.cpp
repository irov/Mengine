#include "Shape.h"

#include "Kernel/Surface.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Shape::Shape()
        : m_surfaceRevision( 0 )
        , m_invalidateVerticesLocal( true )
        , m_invalidateVerticesWM( true )
        , m_invalidateVerticesColor( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Shape::~Shape()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::setSurface( const SurfacePtr & _surface )
    {
        if( m_surface == _surface )
        {
            return;
        }

        bool surfaceActivated = this->isActivate();

        if( surfaceActivated == true )
        {
            m_surface->deactivate();
        }

        this->recompile( [this, &_surface]()
        {
            m_surface = _surface;
            m_surfaceRevision = 0;

            if( m_surface == nullptr )
            {
                return false;
            }

            return true;
        } );

        if( surfaceActivated == true && this->isActivate() == true )
        {
            m_surface->activate();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const SurfacePtr & Shape::getSurface() const
    {
        return m_surface;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::box2f * Shape::getBoundingBox() const
    {
        this->synchronizeSurfaceRevision();

        return BoundingBox::getBoundingBox();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Shape::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_surface, "'%s' can`t setup surface"
            , this->getName().c_str()
        );

        if( m_surface->compile() == false )
        {
            LOGGER_ERROR( "shape '%s' can`t compile surface '%s'"
                , this->getName().c_str()
                , m_surface->getName().c_str()
            );

            return false;
        }

        m_surfaceRevision = m_surface->getRevision();

        this->invalidateVerticesLocal();
        this->invalidateVerticesColor();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::_release()
    {
        m_surface->release();
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::_dispose()
    {
        m_surface = nullptr;

        Node::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Shape::_activate()
    {
        m_surface->activate();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::_deactivate()
    {
        m_surface->deactivate();
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::synchronizeSurfaceRevision() const
    {
        uint32_t revision = m_surface->getRevision();

        if( m_surfaceRevision != revision )
        {
            m_surfaceRevision = revision;

            this->invalidateVerticesLocal();
            this->invalidateVerticesColor();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::_invalidateWorldMatrix() const
    {
        this->invalidateVerticesWM();
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::invalidateVerticesLocal() const
    {
        m_invalidateVerticesLocal = true;

        this->invalidateVerticesWM();
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::invalidateVerticesWM() const
    {
        m_invalidateVerticesWM = true;

        this->invalidateBoundingBox();
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::invalidateVerticesColor() const
    {
        m_invalidateVerticesColor = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::_invalidateColor() const
    {
        this->invalidateVerticesColor();
    }
    //////////////////////////////////////////////////////////////////////////
}
