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

	struct MovieFrameLayer
	{
		TVectorMovieFrameSource frames;
	};

	typedef std::vector<MovieFrameLayer> TVectorMovieFrameLayer;
	
	class MovieFramePack
	{
	public:
		MovieFramePack( size_t _size );

	public:
		bool getLayerFrame( size_t _layerIndex, size_t _frameIndex, MovieFrameSource & _frame ) const;
		bool getLayerFrameLast( size_t _layerIndex, MovieFrameSource & _frame ) const;

		size_t getLayerCountFrames( size_t _layerIndex ) const;

	public:
		bool isLayerEmpty( size_t _layerIndex ) const;

	public:
		const TVectorMovieFrameSource & getLayerFrames( size_t _layerIndex );

	public:
		void addLayerFrame( size_t _layerIndex, const MovieFrameSource & _frame );
			
	protected:
		size_t m_size;
		TVectorMovieFrameLayer m_layers;
	};
}