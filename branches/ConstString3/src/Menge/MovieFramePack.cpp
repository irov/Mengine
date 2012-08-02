#	include "MovieFramePack.h"

namespace Menge
{
	///////////////////////////////////////////////////////////////////////
	MovieFramePack::MovieFramePack( size_t _size )
		: m_size(_size)
	{
		m_layers.resize(m_size);
	}
	//////////////////////////////////////////////////////////////////////////
	void MovieFramePack::initializeLayer( size_t _layerIndex, size_t _count, bool _immutable )
	{
		MovieLayerFrame & frameLayer = m_layers[_layerIndex - 1];

		frameLayer.count = _count;
		frameLayer.immutable = _immutable;

		if( frameLayer.immutable == false && _count != -1 )
		{
			frameLayer.frames.reserve( _count );
		}
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
		if( this->isLayerImmutable( _layerIndex ) == false )
		{
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
		}
		else
		{
			if( this->getLayerImmutableFrame( _layerIndex, _frame ) == false )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MovieFramePack::isLayerEmpty( size_t _layerIndex ) const
	{
		const MovieLayerFrame & frameLayer = m_layers[_layerIndex - 1];

		if( frameLayer.count == -1 )
		{
			if( frameLayer.immutable == true )
			{
				return false;
			}

			return frameLayer.frames.empty() == true;
		}

		return frameLayer.count == 0;
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
		const MovieLayerFrame & frameLayer = m_layers[_layerIndex - 1];

		if( frameLayer.immutable == false )
		{
			const TVectorMovieFrameSource & frameSource = frameLayer.frames;

			if( frameSource.empty() == true )
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
	///////////////////////////////////////////////////////////////////////
}