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
	const uint32_t DATAFLOW_MAGIC_MDL = 0x3D3DBABE;
	const uint32_t DATAFLOW_VERSION_MDL = 1;

#	define MENGINE_MODEL_MAX_VERTEX 1024
#	define MENGINE_MODEL_MAX_INDICES ((MENGINE_MODEL_MAX_VERTEX - 2) * 3)

	struct Model3DFrame
	{
		mt::vec3f cameraPos;
		mt::vec3f cameraDir;
		mt::vec3f cameraUp;

		mt::vec3f pos[MENGINE_MODEL_MAX_VERTEX];
		mt::vec2f uv[MENGINE_MODEL_MAX_VERTEX];
		uint16_t indecies[MENGINE_MODEL_MAX_INDICES];
	};

	typedef std::vector<Model3DFrame> TVectorModel3DFrames;

	class Model3DInterface
		: public DataInterface
	{
	public:
		virtual size_t getFrameCount() const = 0;
		virtual size_t getVertexCount() const = 0;
		virtual size_t getIndicesCount() const = 0;
		virtual float getCameraFOV() const = 0;
		virtual float getCameraAspect() const = 0;
		virtual float getCameraRightSign() const = 0;

	public:
		virtual float getFrameDelay() const = 0;

	public:
		virtual const Model3DFrame * getFrame( size_t _frameId ) const = 0; 
	};

	typedef stdex::intrusive_ptr<Model3DInterface> Model3DInterfacePtr;
}