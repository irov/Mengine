#   pragma once

#   include "Interface/DataInterface.h"

#   include "Config/Floats.h"

#   include "Core/ConstString.h"
#   include "Core/FilePath.h"

#   include "Factory/Factorable.h"

#   include "Math/vec3.h"

#   include <vector>

namespace Menge
{
	const size_t DATAFLOW_VERSION_AEK = 4;

    struct MovieFrameSource
    {
        mt::vec3f anchorPoint;
        mt::vec3f position;
        mt::vec3f rotation;
        mt::vec3f scale;
        float opacity;
        float volume;
    };

    typedef std::vector<MovieFrameSource> TVectorMovieFrameSource;

    struct MovieLayerFrame
    {
        TVectorMovieFrameSource frames;

        MovieFrameSource source;
        bool immutable;
    };

	typedef std::vector<MovieLayerFrame> TVectorMovieFrameLayer;

    struct MovieLayerTimeRemap
    {
        Floats times;
    };

	typedef std::vector<MovieLayerTimeRemap> TVectorMovieLayerTimeRemap;

#	define MENGINE_MOVIE_SHAPE_MAX_VERTEX 32
#	define MENGINE_MOVIE_SHAPE_MAX_INDECIES ((MENGINE_MOVIE_SHAPE_MAX_VERTEX - 2) * 3)

	struct MovieFrameShape
	{
		mt::vec3f pos[MENGINE_MOVIE_SHAPE_MAX_VERTEX];
		mt::vec2f uv[MENGINE_MOVIE_SHAPE_MAX_VERTEX];
		uint16_t indecies[MENGINE_MOVIE_SHAPE_MAX_INDECIES];

		size_t vertexCount;
		size_t indexCount;
	};

	typedef std::vector<MovieFrameShape> TVectorMovieFrameShapes;

	struct MovieLayerShapes
	{
		TVectorMovieFrameShapes shapes;
	};

	typedef std::vector<MovieLayerShapes> TVectorMovieLayerShapes;

	class MovieFramePackInterface
		: public DataInterface
	{
	public:
		virtual bool hasLayer( size_t _layerIndex ) const = 0;
		virtual const MovieLayerFrame & getLayer( size_t _layerIndex ) const = 0;

	public:
		virtual bool getLayerFrame( size_t _layerIndex, size_t _frameIndex, MovieFrameSource & _frame ) const = 0;

	public:
		virtual bool getLayerTimeRemap( size_t _layerIndex, size_t _frameIndex, float & _time ) const = 0;

	public:
		virtual bool getLayerShape( size_t _layerIndex, size_t _frameIndex, const MovieFrameShape ** _shape ) const = 0;
	};

	typedef stdex::intrusive_ptr<MovieFramePackInterface> MovieFramePackInterfacePtr;
}