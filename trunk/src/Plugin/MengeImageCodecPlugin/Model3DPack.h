#	pragma once

#	include "Interface/Model3DInterface.h"

#	include "Config/Floats.h"

#	include "Math/vec2.h"
#	include "Math/vec3.h"

#	include <vector>

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
		void initialize( size_t _frameCount, size_t _vertexCount, size_t _indeciesCount, float _frameDelay );

		Model3DFrame & mutableFrame( size_t _frameId );

	public:
		size_t getFrameCount() const override;
		size_t getVertexCount() const override;
		size_t getIndicesCount() const override;

	public:
		float getFrameDelay() const override;

	public:
		const Model3DFrame & getFrame( size_t _frameId ) const override;
		
	protected:
		size_t m_frameCount;
		size_t m_vertexCount;
		size_t m_indeciesCount;

		float m_frameDelay;

		TVectorModel3DFrames m_model3DFrames;
	};
}