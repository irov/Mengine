#	pragma once
#	include "Math/vec2.h"
#	include "Math/vec3.h"
#	include <vector>

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
		void initialise( size_t _size );

		const MovieFrameSource & getLastLayerFrame( size_t _layerIndex ) const;

		bool isLayerEmpty( size_t _layerIndex ) const;

		void setLayerFrame( size_t _layerIndex, const MovieFrameSource & _frame );

		const MovieFrameSource & getLayerFrame( size_t _layerIndex, size_t _frameIndex ) const;

		size_t getLayerCountFrames( size_t _layerIndex ) const;

		TVectorMovieFrameLayer m_layers;
	protected:
		size_t m_size;

	};


}