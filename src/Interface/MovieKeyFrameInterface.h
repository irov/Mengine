#   pragma once

#   include "Interface/DataInterface.h"
#   include "Interface/RenderSystemInterface.h"

#   include "Config/Floats.h"

#   include "Core/Magic.h"
#   include "Core/ConstString.h"
#   include "Core/FilePath.h"

#   include "Factory/Factorable.h"

#   include "Math/vec3.h"

#   include <stdex/stl_vector.h>

#	include <stdint.h>

namespace Menge
{
	DECLARE_MAGIC_NUMBER(MAGIC_AEK, 'A', 'E', 'K', '1', 28);

    struct MovieFrameSource
    {
        mt::vec3f anchorPoint;
        mt::vec3f position;
        mt::vec3f rotation;
        mt::vec3f scale;
        float opacity;
        float volume;
    };

	const size_t MOVIE_KEY_FRAME_IMMUTABLE_ANCHOR_POINT		= 0x00000001;
	const size_t MOVIE_KEY_FRAME_IMMUTABLE_POSITION			= 0x00000002;
	const size_t MOVIE_KEY_FRAME_IMMUTABLE_ROTATION_X		= 0x00000004;
	const size_t MOVIE_KEY_FRAME_IMMUTABLE_ROTATION_Y		= 0x00000008;
	const size_t MOVIE_KEY_FRAME_IMMUTABLE_ROTATION_Z		= 0x00000010;
	const size_t MOVIE_KEY_FRAME_IMMUTABLE_SCALE			= 0x00000020;
	const size_t MOVIE_KEY_FRAME_IMMUTABLE_OPACITY			= 0x00000040;
	const size_t MOVIE_KEY_FRAME_IMMUTABLE_VOLUME			= 0x00000080;
	
    struct MovieLayerFrame
    {
		size_t count;		
		
		mt::vec3f * anchorPoint;
		mt::vec3f * position;
		mt::vec3f * scale;
		float * rotation_x;
		float * rotation_y;
		float * rotation_z;		
		float * opacity;
		float * volume;

		size_t immutable_mask;
		
        MovieFrameSource source;
        bool immutable;
    };

	typedef stdex::vector<MovieLayerFrame> TVectorMovieFrameLayer;

    struct MovieLayerTimeRemap
    {
		uint32_t layerId;

        Floats times;
    };

	typedef stdex::vector<MovieLayerTimeRemap> TVectorMovieLayerTimeRemap;

#	ifndef MENGINE_MOVIE_SHAPE_MAX_VERTEX
#	define MENGINE_MOVIE_SHAPE_MAX_VERTEX 32
#	endif

#	define MENGINE_MOVIE_SHAPE_MAX_INDICES ((MENGINE_MOVIE_SHAPE_MAX_VERTEX - 2) * 3)

	struct MovieFrameShape
	{
		mt::vec2f pos[MENGINE_MOVIE_SHAPE_MAX_VERTEX];
		mt::vec2f uv[MENGINE_MOVIE_SHAPE_MAX_VERTEX];
		RenderIndices2D indices[MENGINE_MOVIE_SHAPE_MAX_INDICES];

		uint8_t vertexCount;
		uint8_t indexCount;
	};

	typedef stdex::vector<MovieFrameShape> TVectorMovieFrameShapes;

	struct MovieLayerShapes
	{
		uint32_t layerId;

		TVectorMovieFrameShapes shapes;
	};

	typedef stdex::vector<MovieLayerShapes> TVectorMovieLayerShapes;

#	ifndef MENGINE_MOVIE_POLYGON_MAX_VERTEX
#	define MENGINE_MOVIE_POLYGON_MAX_VERTEX 32
#	endif
	
	struct MovieLayerPolygon
	{
		uint32_t layerId;

		mt::vec2f polygon[MENGINE_MOVIE_POLYGON_MAX_VERTEX];
		uint8_t vertexCount;
	};

	typedef stdex::vector<MovieLayerPolygon> TVectorMovieLayerPolygons;

	class MovieFramePackInterface
		: public DataInterface
	{
	public:
		virtual bool hasLayer( size_t _layerIndex ) const = 0;
		virtual const MovieLayerFrame & getLayer( size_t _layerIndex ) const = 0;

	public:
		virtual bool getLayerFrame( size_t _layerIndex, size_t _frameIndex, MovieFrameSource & _frame ) const = 0;
		virtual bool getLayerFrameInterpolate( size_t _layerIndex, size_t _frameIndex, float _t, MovieFrameSource & _frame ) const = 0;

	public:
		virtual bool getLayerTimeRemap( size_t _layerIndex, size_t _frameIndex, float & _time ) const = 0;
		virtual bool getLayerShape( size_t _layerIndex, size_t _frameIndex, const MovieFrameShape ** _shape ) const = 0;
		virtual bool getLayerPolygon( size_t _layerIndex, const mt::vec2f ** _polygon, uint8_t & _vertexCount ) const = 0;

	public:
		virtual bool isLayerPermanentlyHide( size_t _layerIndex ) const = 0;
	};

	typedef stdex::intrusive_ptr<MovieFramePackInterface> MovieFramePackInterfacePtr;
}