#   include "MovieEvent.h"

#   include "pybind/pybind.hpp"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    MovieEvent::MovieEvent()
        : m_cb(NULL)
        , m_resourceMovie(NULL)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MovieEvent::~MovieEvent()
    {
        if( m_cb != NULL )
        {
            pybind::decref( m_cb );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieEvent::setEvent( PyObject * _cb )
    {
        if( m_cb != NULL )
        {
            pybind::decref( m_cb );
        }

        m_cb = _cb;

        if( m_cb != NULL )
        {
            pybind::incref( m_cb );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieEvent::setResourceMovie( ResourceMovie * _resourceMovie )
    {
        m_resourceMovie = _resourceMovie;
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieEvent::_movieForwardUpdate( float _time, size_t _beginFrame, size_t _endFrame, const MovieLayer & _layer )
    {
        (void)_time;

        if( m_cb == NULL )
        {
            return;
        }
        
        float layerIn = _layer.in;
        float layerOut = _layer.out;

        float frameDuration = m_resourceMovie->getFrameDuration();

        size_t indexIn = (size_t)((layerIn / frameDuration) + 0.5f);
        size_t indexOut = (size_t)((layerOut / frameDuration) + 0.5f);

        if( indexIn > _endFrame || indexOut < _beginFrame )
        {
            return;
        }

        if( _beginFrame <= indexIn && _endFrame >= indexIn )
        {
            MovieFrameSource frame;
            if( m_resourceMovie->getFrame( _layer, indexIn, frame ) == false )
            {
                return;
            }
            
            SCRIPT_SERVICE(m_serviceProvider)
                ->callFunction( m_cb, "(O)", pybind::ptr(frame.position) );
        }
    }
}