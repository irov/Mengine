#	pragma once

#	include "Interface/Model3DInterface.h"

#	include "Config/Floats.h"

#	include "Math/vec2.h"
#	include "Math/vec3.h"

#	include <stdex/stl_vector.h>

#	include <stdint.h>
#	include <memory.h>

namespace Menge
{       
	class Model3DPack
		: public Model3DInterface
	{
	public:
		Model3DPack();
        
	public:
		void initialize( uint32_t _frameCount, uint32_t _vertexCount, uint32_t _indeciesCount, float _frameDelay );
		void setCamera( float _fov, float _aspect, float _rightSign );

		Model3DFrame & mutableFrame( uint32_t _frameId );

	public:
		uint32_t getFrameCount() const override;
		uint32_t getVertexCount() const override;
		uint32_t getIndicesCount() const override;
		float getCameraFOV() const override;
		float getCameraAspect() const override;
		float getCameraRightSign() const override;

	public:
		float getFrameDelay() const override;

	public:
		const Model3DFrame * getFrame( uint32_t _frameId ) const override;
		
	protected:
		uint32_t m_frameCount;
		uint32_t m_vertexCount;
		uint32_t m_indicesCount;

		float m_cameraFOV;
		float m_cameraAspect;
		float m_cameraRightSign;

		float m_frameDelay;

		TVectorModel3DFrames m_model3DFrames;
	};
}