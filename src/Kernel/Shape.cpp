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
    }
    //////////////////////////////////////////////////////////////////////////
    const SurfacePtr & Shape::getSurface() const
    {
        return m_surface;
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
    void Shape::update( const UpdateContext * _context )
    {
        uint32_t revision = m_surface->update( _context );

        if( m_surfaceRevision < revision )
        {
            this->invalidateVerticesLocal();
            this->invalidateVerticesColor();

            m_surfaceRevision = revision;
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