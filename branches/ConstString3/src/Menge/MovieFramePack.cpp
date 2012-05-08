#	include "MovieFramePack.h"

namespace Menge
{
	///////////////////////////////////////////////////////////////////////
	void MovieFramePack::initialise( size_t _size )
	{
		m_size = _size;
		m_layers.resize(m_size);
	}
	///////////////////////////////////////////////////////////////////////
	const MovieFrameSource & MovieFramePack::getLastLayerFrame( size_t _layerIndex ) const
	{
		const MovieFrameLayer & frameLayer = m_layers[_layerIndex - 1];
		const TVectorMovieFrameSource & frameSource = frameLayer.frames;
		const MovieFrameSource & frame = frameSource.back();
		
		return frame;
	}
	///////////////////////////////////////////////////////////////////////
	bool MovieFramePack::isLayerEmpty( size_t _layerIndex ) const
	{
		const MovieFrameLayer & frameLayer = m_layers[_layerIndex - 1];
		const TVectorMovieFrameSource & frameSource = frameLayer.frames;
		bool isEmpty = frameSource.empty();
		
		return isEmpty;
	}
	///////////////////////////////////////////////////////////////////////
	void MovieFramePack::setLayerFrame( size_t _layerIndex, const MovieFrameSource & _frame )
	{
		MovieFrameLayer & frameLayer = m_layers[_layerIndex - 1];
		TVectorMovieFrameSource & frameSource = frameLayer.frames;
		frameSource.push_back( _frame );
	}
	///////////////////////////////////////////////////////////////////////
	const MovieFrameSource & MovieFramePack::getLayerFrame( size_t _layerIndex, size_t _frameIndex ) const
	{
		const MovieFrameLayer & frameLayer = m_layers[_layerIndex - 1];
		const TVectorMovieFrameSource & frameSource = frameLayer.frames;
		const MovieFrameSource & frame =  frameSource[_frameIndex];
		
		return frame;
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