#include "MovieInternalObject.h"

#include "Environment/Python/PythonIncluder.h"

#include "Movie.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/EventableHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MovieInternalObject::MovieInternalObject()
        : m_movie( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MovieInternalObject::~MovieInternalObject()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieInternalObject::setMovie( Movie * _movie )
    {
        m_movie = _movie;
    }
    //////////////////////////////////////////////////////////////////////////
    Movie * MovieInternalObject::getMovie() const
    {
        return m_movie;
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieInternalObject::setResourceInternalObject( const ResourceInternalObjectPtr & _resourceInternalObject )
    {
        if( m_resourceInternalObject == _resourceInternalObject )
        {
            return;
        }

        m_resourceInternalObject = _resourceInternalObject;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceInternalObjectPtr & MovieInternalObject::getResourceInternalObject() const
    {
        return m_resourceInternalObject;
    }
    //////////////////////////////////////////////////////////////////////////
    const NodePtr & MovieInternalObject::getInternalNode() const
    {
        return m_internalNode;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MovieInternalObject::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_movie, "'%s' movie not setup"
            , this->getName().c_str()
        );

        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceInternalObject, "'%s' resource not setup"
            , this->getName().c_str()
        );

        if( m_resourceInternalObject->compile() == false )
        {
            LOGGER_ERROR( "'%s' resource '%s' not compile"
                , this->getName().c_str()
                , m_resourceInternalObject->getName().c_str()
            );

            return false;
        }

        const ConstString & internalGroup = m_resourceInternalObject->getInternalGroup();
        const ConstString & internalName = m_resourceInternalObject->getInternalName();

        pybind::object py_object = EVENTABLE_OTHER_METHODR( m_movie, EVENT_MOVIE_GET_INTERNAL, pybind::object() )
            ->onMovieGetInternal( internalGroup, internalName );

        if( py_object.is_invalid() == true )
        {
            LOGGER_ERROR( "'%s' resource '%s' can't find internal object internal group '%s' name '%s'"
                , this->getName().c_str()
                , m_resourceInternalObject->getName().c_str()
                , internalGroup.c_str()
                , internalName.c_str()
            );

            return false;
        }

        m_internalObject = py_object;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieInternalObject::_release()
    {
        m_resourceInternalObject->release();

        m_internalObject.reset();
    }
    //////////////////////////////////////////////////////////////////////////
    bool MovieInternalObject::_activate()
    {
        if( Node::_activate() == false )
        {
            return false;
        }

        NodePtr node = EVENTABLE_OTHER_METHODR( m_movie, EVENT_MOVIE_ACTIVATE_INTERNAL, nullptr )
            ->onMovieActivateInternal( m_internalObject );

        MENGINE_ASSERTION_MEMORY_PANIC( node, "'%s' resource '%s' invalid get internal node group '%s' name '%s'"
            , this->getName().c_str()
            , m_resourceInternalObject->getName().c_str()
            , m_resourceInternalObject->getInternalGroup().c_str()
            , m_resourceInternalObject->getInternalName().c_str()
        );

        this->addChild( node );

        TransformationInterface * transformation = node->getTransformation();

        transformation->resetTransformation();

        m_internalNode = node;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieInternalObject::_deactivate()
    {
        Node::_deactivate();

        m_internalNode->dispose();
        m_internalNode = nullptr;

        EVENTABLE_OTHER_METHOD( m_movie, EVENT_MOVIE_DEACTIVATE_INTERNAL )
            ->onMovieDeactivateInternal( m_internalObject );
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieInternalObject::_setLocalHide( bool _hide )
    {
        this->setHide( _hide );
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieInternalObject::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        MENGINE_UNUSED( _renderPipeline );
        MENGINE_UNUSED( _context );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieInternalObject::_setPersonalColor( const Color & _color )
    {
        this->setLocalColor( _color );
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieInternalObject::_setPersonalAlpha( float _alpha )
    {
        this->setLocalAlpha( _alpha );
    }
    //////////////////////////////////////////////////////////////////////////
}