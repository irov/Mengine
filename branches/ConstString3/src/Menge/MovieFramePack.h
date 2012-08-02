#	pragma once

#	include "Math/vec2.h"
#	include "Math/vec3.h"

#	include <vector>

#	include <memory.h>

namespace Menge
{
	struct MovieFrameSource
	{
		mt::vec3f anchorPoint;
		mt::vec3f position;
		mt::vec3f rotation;
		mt::vec3f scale;
		float opacity;
	};
	
	typedef std::vector<MovieFrameSource> TVectorMovieFrameSource;

	struct MovieLayerFrame
	{	
		size_t count;
		TVectorMovieFrameSource frames;

		MovieFrameSource source;
		bool immutable;
	};

	typedef std::vector<MovieLayerFrame> TVectorMovieFrameLayer;
	
	class MovieFramePack
	{
	public:
		MovieFramePack( size_t _size );

	public:
		void initializeLayer( size_t _layerIndex, size_t _count, bool _immutable );

	public:
		const MovieLayerFrame & getLayer( size_t _layerIndex ) const;

	public:
		bool getLayerFrame( size_t _layerIndex, size_t _frameIndex, MovieFrameSource & _frame ) const;
		bool getLayerFrameLast( size_t _layerIndex, MovieFrameSource & _frame ) const;

		bool getLayerImmutableFrame( size_t _layerIndex, MovieFrameSource & _frame ) const;

	public:
		bool isLayerEmpty( size_t _layerIndex ) const;
		bool isLayerImmutable( size_t _layerIndex ) const;

	public:
		void setLayerImmutableFrame( size_t _layerIndex, const MovieFrameSource & _frame );
		void addLayerFrame( size_t _layerIndex, const MovieFrameSource & _frame );
			
	protected:
		size_t m_size;
		TVectorMovieFrameLayer m_layers;
	};
}