#pragma once

#include "Interface/RenderSystemInterface.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class RenderMaterial
		: public RenderMaterialInterface
	{
	public:
		RenderMaterial();
		~RenderMaterial() override;

	public:
		void initialize( const ConstString & _name, uint32_t _id, uint32_t _hash, EPrimitiveType _primitiveType, uint32_t _textureCount, const RenderTextureInterfacePtr * _textures, const RenderMaterialStage * _stage );
		
	public:
		const ConstString & getName() const override;
		uint32_t getId() const override;

	public:
		void update( float _time, float _timing ) override;

	public:
		inline uint32_t getHash() const;
		inline EPrimitiveType getPrimitiveType() const override;
		inline uint32_t getTextureCount() const override;
		inline const RenderTextureInterfacePtr & getTexture( uint32_t _index ) const override;
		inline const RenderMaterialStage * getStage() const override;

	protected:
		ConstString m_name;

		uint32_t m_id;
		uint32_t m_hash;

		EPrimitiveType m_primitiveType;

		uint32_t m_textureCount;
		RenderTextureInterfacePtr m_textures[MENGINE_MAX_TEXTURE_STAGES];

		const RenderMaterialStage * m_stage;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const ConstString & RenderMaterial::getName() const
	{ 
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	inline uint32_t RenderMaterial::getId() const
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
	inline uint32_t RenderMaterial::getTextureCount() const
	{
		return m_textureCount;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const RenderTextureInterfacePtr & RenderMaterial::getTexture( uint32_t _index ) const
	{
		return m_textures[_index];
	}
	//////////////////////////////////////////////////////////////////////////
	inline const RenderMaterialStage * RenderMaterial::getStage() const
	{
		return m_stage;
	}
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<RenderMaterial> RenderMaterialPtr;
}