#	include "RenderMaterial.h"

#	include "Interface/TextInterface.h"

#	include "stdex/hash.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderMaterial::RenderMaterial()
		: m_id(0)
		, m_hash(0)
		, m_primitiveType(PT_TRIANGLELIST)
		, m_textureCount(0)
		, m_stage(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	RenderMaterial::~RenderMaterial()
	{
		for( size_t i = 0; i != m_textureCount; ++i )
		{
			m_textures[i] = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMaterial::initialize( size_t _id, EPrimitiveType _primitiveType, size_t _textureCount, const RenderTextureInterfacePtr * _textures, const RenderStage * _stage )
	{
		m_id = _id;
		m_primitiveType = _primitiveType;

		m_textureCount = _textureCount;

		for( size_t i = 0; i != m_textureCount; ++i )
		{
			m_textures[i] = _textures[i];
		}

		m_stage = _stage;

		m_hash = stdex::hash_base;

		m_hash = stdex::hash_binary( m_hash, m_primitiveType );
		m_hash = stdex::hash_binary( m_hash, m_textureCount );

		for( size_t i = 0; i != m_textureCount; ++i )
		{
			m_hash = stdex::hash_binary( m_hash, m_textures[i].get() );
		}

		m_hash = stdex::hash_binary( m_hash, m_stage );
	}
}