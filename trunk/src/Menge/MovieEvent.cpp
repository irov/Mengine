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
	}
    //////////////////////////////////////////////////////////////////////////
	void MovieEvent::setEvent( const pybind::object & _cb )
    {
        m_cb = _cb;
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
			if( framePack->getLayerFrame( _layer.index, 0, frame ) == false )
            {
                return;
            }
            
            m_cb( frame.position );
        }
    }
}