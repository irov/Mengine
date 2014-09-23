#   pragma once

#   include "Interface/DataInterface.h"
#   include "Interface/RenderSystemInterface.h"

#   include "Config/Floats.h"

#	include "Core/Magic.h"
#   include "Core/ConstString.h"
#   include "Core/FilePath.h"

#   include "Factory/Factorable.h"

#   include "Math/vec3.h"

#	include <stdex/stl_vector.h>

namespace Menge
{
	DECLARE_MAGIC_NUMBER(MAGIC_MDL, 'M', 'D', 'L', '1', 1);

#	define MENGINE_MODEL_MAX_VERTEX 1024
#	define MENGINE_MODEL_MAX_INDICES ((MENGINE_MODEL_MAX_VERTEX - 2) * 3)

	struct Model3DFrame
	{
		mt::vec3f cameraPos;
		mt::vec3f cameraDir;
		mt::vec3f cameraUp;

		mt::vec3f pos[MENGINE_MODEL_MAX_VERTEX];
		mt::vec2f uv[MENGINE_MODEL_MAX_VERTEX];
		RenderIndices2D indecies[MENGINE_MODEL_MAX_INDICES];
	};

	typedef stdex::vector<Model3DFrame> TVectorModel3DFrames;

	class Model3DInterface
		: public DataInterface
	{
	public:
		virtual uint32_t getFrameCount() const = 0;
		virtual uint32_t getVertexCount() const = 0;
		virtual uint32_t getIndicesCount() const = 0;
		virtual float getCameraFOV() const = 0;
		virtual float getCameraAspect() const = 0;
		virtual float getCameraRightSign() const = 0;

	public:
		virtual float getFrameDelay() const = 0;

	public:
		virtual const Model3DFrame * getFrame( uint32_t _frameId ) const = 0; 
	};

	typedef stdex::intrusive_ptr<Model3DInterface> Model3DInterfacePtr;
}