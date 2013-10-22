#	include "MovieFramePack.h"

namespace Menge
{
	///////////////////////////////////////////////////////////////////////
	MovieFramePack::MovieFramePack()
	{		
	}
    //////////////////////////////////////////////////////////////////////////
    void MovieFramePack::initialize( size_t _size )
    {
        m_layers.resize( _size );
        m_timeremap.resize( _size );
    }
	//////////////////////////////////////////////////////////////////////////
	void MovieFramePack::initializeLayer( size_t _layerIndex, size_t _count, bool _immutable )
	{
		MovieLayerFrame & frameLayer = m_layers[_layerIndex - 1];

		frameLayer.immutable = _immutable;

		if( frameLayer.immutable == false )
		{
			frameLayer.frames.reserve( _count );
		}
	}
    //////////////////////////////////////////////////////////////////////////
    bool MovieFramePack::hasLayer( size_t _layerIndex ) const
    {
        if( (_layerIndex - 1) >= m_layers.size() )
        {
            return false;
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	const MovieLayerFrame & MovieFramePack::getLayer( size_t _layerIndex ) const
	{
		const MovieLayerFrame & frameLayer = m_layers[_layerIndex - 1];

		return frameLayer;
	}
	///////////////////////////////////////////////////////////////////////
	void MovieFramePack::setLayerImmutableFrame( size_t _layerIndex, const MovieFrameSource & _frame )
	{
		MovieLayerFrame & frameLayer = m_layers[_layerIndex - 1];
		
		frameLayer.source = _frame;
	}
	///////////////////////////////////////////////////////////////////////
	void MovieFramePack::addLayerFrame( size_t _layerIndex, const MovieFrameSource & _frame )
	{
		MovieLayerFrame & frameLayer = m_layers[_layerIndex - 1];
		
		TVectorMovieFrameSource & frameSource = frameLayer.frames;

		frameSource.push_back( _frame );
	}
	//////////////////////////////////////////////////////////////////////////
	TVectorMovieFrameSource & MovieFramePack::mutableLayerFrames( size_t _layerIndex )
	{
		MovieLayerFrame & frameLayer = m_layers[_layerIndex - 1];

		return frameLayer.frames;
	}
    //////////////////////////////////////////////////////////////////////////
    void MovieFramePack::addLayerTimeRemap( size_t _layerIndex, const MovieLayerTimeRemap & _timeremap )
    {
        m_timeremap[_layerIndex - 1] = _timeremap;
    }
	//////////////////////////////////////////////////////////////////////////
	MovieLayerTimeRemap & MovieFramePack::mutableLayerTimeRemap( size_t _layerIndex )
	{
		MovieLayerTimeRemap & timeremap = m_timeremap[_layerIndex - 1];

		return timeremap;
	}
	//////////////////////////////////////////////////////////////////////////
	void MovieFramePack::addLayerShape( size_t _layerIndex, const MovieLayerShapes & _shapes )
	{
		m_shapes[_layerIndex - 1] = _shapes;
	}
	//////////////////////////////////////////////////////////////////////////
	MovieLayerShapes & MovieFramePack::mutableLayerShape( size_t _layerIndex )
	{
		MovieLayerShapes & shapes = m_shapes[_layerIndex - 1];

		return shapes;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MovieFramePack::getLayerImmutableFrame( size_t _layerIndex, MovieFrameSource & _frame ) const
	{
		const MovieLayerFrame & frameLayer = m_layers[_layerIndex - 1];

		if( frameLayer.immutable == false )
		{
			return false;
		}

		_frame = frameLayer.source;

		return true;
	}
	///////////////////////////////////////////////////////////////////////
	bool MovieFramePack::getLayerFrameLast( size_t _layerIndex, MovieFrameSource & _frame ) const
	{
		if( this->isLayerImmutable( _layerIndex ) == true )
		{
            return false;
        }
		
        const MovieLayerFrame & frameLayer = m_layers[_layerIndex - 1];

        if( frameLayer.frames.empty() == true )
        {
            return false;
        }

        size_t lastIndex = frameLayer.frames.size() - 1;

        if( this->getLayerFrame( _layerIndex, lastIndex, _frame ) == false )
        {
            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MovieFramePack::isLayerEmpty( size_t _layerIndex ) const
	{
		const MovieLayerFrame & frameLayer = m_layers[_layerIndex - 1];

		if( frameLayer.immutable == true )
		{
            return false;
        }

		return frameLayer.frames.empty() == true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MovieFramePack::isLayerImmutable( size_t _layerIndex ) const
	{
		const MovieLayerFrame & frameLayer = m_layers[_layerIndex - 1];
		
		bool immutable = frameLayer.immutable;

		return immutable;
	}
	///////////////////////////////////////////////////////////////////////
	bool MovieFramePack::getLayerFrame( size_t _layerIndex, size_t _frameIndex, MovieFrameSource & _frame ) const
	{
        if( (_layerIndex - 1) >= m_layers.size() )
        {
            return false;
        }

		const MovieLayerFrame & frameLayer = m_layers[_layerIndex - 1];

		if( frameLayer.immutable == false )
		{
			const TVectorMovieFrameSource & frameSource = frameLayer.frames;

            if( _frameIndex >= frameSource.size() )
            {
                return false;
            }
		
			_frame =  frameSource[_frameIndex];
		}
		else
		{
			_frame = frameLayer.source;
		}
		
		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool MovieFramePack::getLayerTimeRemap( size_t _layerIndex, size_t _frameIndex, float & _time ) const
    {
        const MovieLayerTimeRemap & timeremap = m_timeremap[_layerIndex - 1];

        float time = timeremap.times[_frameIndex];

        _time = time;

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MovieFramePack::getLayerShape( size_t _layerIndex, size_t _frameIndex, const MovieFrameShape ** _shape ) const
	{
		const MovieLayerShapes & shapes = m_shapes[_layerIndex - 1];

		const MovieFrameShape & shape = shapes.shapes[_frameIndex];

		*_shape = &shape;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const TVectorMovieFrameLayer & MovieFramePack::getLayers() const
	{
		return m_layers;
	}
	//////////////////////////////////////////////////////////////////////////
	const TVectorMovieLayerTimeRemap & MovieFramePack::getTimeremap() const
	{
		return m_timeremap;
	}
	//////////////////////////////////////////////////////////////////////////
	const TVectorMovieLayerShapes & MovieFramePack::getShapes() const
	{
		return m_shapes;
	}
}