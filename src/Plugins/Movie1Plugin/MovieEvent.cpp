#include "MovieEvent.h"

#include "Environment/Python/PythonIncluder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MovieEvent::MovieEvent()
        : m_resourceMovie( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MovieEvent::~MovieEvent()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieEvent::setEvent( const pybind::object & _cb, const pybind::args & _args )
    {
        m_cb = _cb;
        m_args = _args;
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieEvent::removeEvent()
    {
        if( m_cb.is_invalid() == false && m_cb.is_callable() == true )
        {
            pybind::object cb = m_cb;
            pybind::args args = m_args;

            m_cb.reset();
            m_args.reset();

            cb.call_args( 0.f, false, args );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieEvent::setResourceMovie( const ResourceMoviePtr & _resourceMovie )
    {
        m_resourceMovie = _resourceMovie;
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieEvent::_movieForwardUpdate( float _time, uint32_t _beginFrame, uint32_t _endFrame, const MovieLayer & _layer )
    {
        MENGINE_UNUSED( _time );

        if( m_cb.is_invalid() == true )
        {
            return;
        }

        float layerIn = _layer.in;
        float layerOut = _layer.out;

        float frameDuration = m_resourceMovie->getFrameDuration();

        uint32_t indexIn = (uint32_t)((layerIn / frameDuration) + 0.5f);
        uint32_t indexOut = (uint32_t)((layerOut / frameDuration) + 0.5f);

        if( indexIn > _endFrame || indexOut < _beginFrame )
        {
            return;
        }

        if( _beginFrame < indexIn && _endFrame >= indexIn )
        {
            const MovieFramePackInterfacePtr & framePack = m_resourceMovie->getFramePack();

            MovieFrameSource frame;
            if( framePack->getLayerFrame( _layer.index, 0, &frame ) == false )
            {
                return;
            }

            m_cb.call_args( frame.position, true, m_args );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}