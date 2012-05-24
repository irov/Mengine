#	include "MovieFramePack.h"

namespace Menge
{
	///////////////////////////////////////////////////////////////////////
	MovieFramePack::MovieFramePack( size_t _size )
		: m_size(_size)
	{
		m_layers.resize(m_size);
	}
	///////////////////////////////////////////////////////////////////////
	void MovieFramePack::addLayerFrame( size_t _layerIndex, const MovieFrameSource & _frame )
	{
		MovieFrameLayer & frameLayer = m_layers[_layerIndex - 1];
		
		TVectorMovieFrameSource & frameSource = frameLayer.frames;

		frameSource.push_back( _frame );
	}
	///////////////////////////////////////////////////////////////////////
	bool MovieFramePack::getLayerFrameLast( size_t _layerIndex, MovieFrameSource & _frame ) const
	{
		size_t count = this->getLayerCountFrames( _layerIndex );

		size_t lastIndex = count - 1;

		if( this->getLayerFrame( _layerIndex, lastIndex, _frame ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MovieFramePack::isLayerEmpty( size_t _layerIndex ) const
	{
		const MovieFrameLayer & frameLayer = m_layers[_layerIndex - 1];
		const TVectorMovieFrameSource & frameSource = frameLayer.frames;

		if( frameSource.empty() == true )
		{
			return true;
		}

		return false;
	}
	///////////////////////////////////////////////////////////////////////
	bool MovieFramePack::getLayerFrame( size_t _layerIndex, size_t _frameIndex, MovieFrameSource & _frame ) const
	{
		const MovieFrameLayer & frameLayer = m_layers[_layerIndex - 1];
		const TVectorMovieFrameSource & frameSource = frameLayer.frames;

		if( frameSource.empty() == true )
		{
			return false;
		}
		
		_frame =  frameSource[_frameIndex];
		
		return true;
	}
	///////////////////////////////////////////////////////////////////////
	size_t MovieFramePack::getLayerCountFrames( size_t _layerIndex ) const
	{
		const MovieFrameLayer & frameLayer = m_layers[_layerIndex - 1];
		const TVectorMovieFrameSource & frameSource = frameLayer.frames;
		size_t size = frameSource.size();

		return size;
	}
	///////////////////////////////////////////////////////////////////////
	const TVectorMovieFrameSource & MovieFramePack::getLayerFrames( size_t _layerIndex )
	{
		const MovieFrameLayer & frameLayer = m_layers[_layerIndex - 1];
		const TVectorMovieFrameSource & frameSource = frameLayer.frames;

		return frameSource;
	}
	///////////////////////////////////////////////////////////////////////
}