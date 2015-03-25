#   include "MovieEvent.h"

#   include "pybind/pybind.hpp"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    MovieEvent::MovieEvent()
        : m_cb(nullptr)
        , m_resourceMovie(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MovieEvent::~MovieEvent()
    {
		pybind::decref( m_cb );
		m_cb = nullptr;
	}
    //////////////////////////////////////////////////////////////////////////
    void MovieEvent::setEvent( PyObject * _cb )
    {
		pybind::decref( m_cb );
  
        m_cb = _cb;

		pybind::incref( m_cb );
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieEvent::setResourceMovie( ResourceMovie * _resourceMovie )
    {
        m_resourceMovie = _resourceMovie;
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieEvent::_movieForwardUpdate( float _time, uint32_t _beginFrame, uint32_t _endFrame, const MovieLayer & _layer )
    {
        (void)_time;

        if( m_cb == nullptr )
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

        if( _beginFrame <= indexIn && _endFrame >= indexIn )
        {
			const MovieFramePackInterfacePtr & framePack = m_resourceMovie->getFramePack();

            MovieFrameSource frame;
            if( framePack->getLayerFrame( _layer.index, indexIn, frame ) == false )
            {
                return;
            }
            
            pybind::call( m_cb, "(N)"
				, pybind::ptr(frame.position) 
				);
        }
    }
}