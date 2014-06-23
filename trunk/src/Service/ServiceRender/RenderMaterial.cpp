#	include "RenderMaterial.h"

#	include "Interface/TextInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderMaterial::RenderMaterial()
		: m_id(0)
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
	void RenderMaterial::initialize( size_t _id, EPrimitiveType _primitiveType, const RenderStage * _stage )
	{
		m_id = _id;
		m_primitiveType = _primitiveType;
		m_stage = _stage;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMaterial::setTexture( size_t _textureCount, const RenderTextureInterfacePtr * _textures )
	{
		m_textureCount = _textureCount;

		for( size_t i = 0; i != m_textureCount; ++i )
		{
			m_textures[i] = _textures[i];
		}
	}
}