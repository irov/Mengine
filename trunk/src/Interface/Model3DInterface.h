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
	const size_t DATAFLOW_VERSION_MDL = 1;

#	define MENGINE_MOVIE_MODEL_MAX_VERTEX 64
#	define MENGINE_MOVIE_MODEL_MAX_INDECIES ((MENGINE_MOVIE_MODEL_MAX_VERTEX - 2) * 3)

	struct Model3DFrame
	{
		mt::vec3f cameraPos; 
		mt::vec3f cameraDir;

		mt::vec3f pos[MENGINE_MOVIE_MODEL_MAX_VERTEX];
		mt::vec2f uv[MENGINE_MOVIE_MODEL_MAX_VERTEX];
		uint16_t indecies[MENGINE_MOVIE_MODEL_MAX_INDECIES];
	};

	typedef std::vector<Model3DFrame> TVectorModel3DFrames;

	class Model3DInterface
		: public DataInterface
	{
	public:
		virtual size_t getFrameCount() const = 0;
		virtual size_t getVertexCount() const = 0;
		virtual size_t getIndicesCount() const = 0;

	public:
		virtual float getFrameDelay() const = 0;

	public:
		virtual const Model3DFrame & getFrame( size_t _frameId ) const = 0; 
	};

	typedef stdex::intrusive_ptr<Model3DInterface> Model3DInterfacePtr;
}