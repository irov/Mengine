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
		void initialize( size_t _id, EPrimitiveType _primitiveType, const RenderStage * _stage );

	public:
		void setTexture( size_t _textureCount, const RenderTextureInterfacePtr * _textures );

	public:
		inline size_t getId() const;
		inline EPrimitiveType getPrimitiveType() const;
		inline size_t getTextureCount() const;
		inline const RenderTextureInterfacePtr & getTexture( size_t _index ) const;
		inline const RenderTextureInterfacePtr * getTextures() const;

		inline const RenderStage * getStage() const;

	protected:
		size_t m_id;
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