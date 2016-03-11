#	include "RenderMaterial.h"

#	include "Interface/TextInterface.h"

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
		for( uint32_t i = 0; i != m_textureCount; ++i )
		{
			m_textures[i] = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMaterial::initialize( const ConstString & _name, uint32_t _id, uint32_t _hash, EPrimitiveType _primitiveType, uint32_t _textureCount, const RenderTextureInterfacePtr * _textures, const RenderStage * _stage )
	{
		m_name = _name;

		m_id = _id;
		m_hash = _hash;
		m_primitiveType = _primitiveType;
		m_stage = _stage;

		m_textureCount = _textureCount;

		for( uint32_t i = 0; i != m_textureCount; ++i )
		{
			m_textures[i] = _textures[i];
		}
	}
}