#	pragma once

#   include "Interface/RenderSystemInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class RenderMaterial
		: public RenderMaterialInterface
	{
	public:
		RenderMaterial();
		~RenderMaterial();

	public:
		void initialize( size_t _id, uint32_t _hash, EPrimitiveType _primitiveType, size_t _textureCount, const RenderTextureInterfacePtr * _textures, const RenderStage * _stage );
		
	public:
		inline size_t getId() const;
		inline uint32_t getHash() const;
		inline EPrimitiveType getPrimitiveType() const;
		inline size_t getTextureCount() const;
		inline const RenderTextureInterfacePtr & getTexture( size_t _index ) const;
		inline const RenderTextureInterfacePtr * getTextures() const;

		inline const RenderStage * getStage() const;

	protected:
		size_t m_id;
		uint32_t m_hash;

		EPrimitiveType m_primitiveType;

		size_t m_textureCount;
		RenderTextureInterfacePtr m_textures[MENGE_MAX_TEXTURE_STAGES];

		const RenderStage * m_stage;
	};
	//////////////////////////////////////////////////////////////////////////
	inline size_t RenderMaterial::getId() const
	{
		return m_id;
	}
	//////////////////////////////////////////////////////////////////////////
	inline uint32_t RenderMaterial::getHash() const
	{
		return m_hash;
	}
	//////////////////////////////////////////////////////////////////////////
	inline EPrimitiveType RenderMaterial::getPrimitiveType() const
	{
		return m_primitiveType;
	}
	//////////////////////////////////////////////////////////////////////////
	inline size_t RenderMaterial::getTextureCount() const
	{
		return m_textureCount;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const RenderTextureInterfacePtr & RenderMaterial::getTexture( size_t _index ) const
	{
		return m_textures[_index];
	}
	//////////////////////////////////////////////////////////////////////////
	inline const RenderTextureInterfacePtr * RenderMaterial::getTextures() const
	{
		return m_textures;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const RenderStage * RenderMaterial::getStage() const
	{
		return m_stage;
	}
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<RenderMaterial> RenderMaterialPtr;
}