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
		return m_layers[_layerIndex - 1].frames.back();
	}
	///////////////////////////////////////////////////////////////////////
	bool MovieFramePack::isLayerEmpty( size_t _layerIndex ) const
	{
		bool isEmpty = m_layers[_layerIndex - 1].frames.empty();
		return isEmpty;
	}
	///////////////////////////////////////////////////////////////////////
	void MovieFramePack::setLayerFrame( size_t _layerIndex, const MovieFrameSource & _frame )
	{
		m_layers[_layerIndex - 1].frames.push_back( _frame );
	}
	///////////////////////////////////////////////////////////////////////
	const MovieFrameSource & MovieFramePack::getLayerFrame( size_t _layerIndex, size_t _frameIndex ) const
	{
		return m_layers[_layerIndex - 1].frames[_frameIndex];
	}
	///////////////////////////////////////////////////////////////////////
	size_t MovieFramePack::getLayerCountFrames( size_t _layerIndex ) const
	{
		return m_layers[_layerIndex - 1].frames.size();
	}
	///////////////////////////////////////////////////////////////////////
}