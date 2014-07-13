#	pragma once

#   include "Interface/RenderSystemInterface.h"

#	include "stdex/intrusive_splay_tree.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class RenderMaterial
		: public RenderMaterialInterface
		, public stdex::intrusive_splay_node<RenderMaterial>
	{
	public:
		typedef const RenderMaterial * key_type;

		struct less_type
		{
			bool operator() ( const RenderMaterial * _l, const RenderMaterial * _r ) const
			{
				uint32_t l_hash = _l->m_hash;
				uint32_t r_hash = _r->m_hash;

				if( l_hash < r_hash )
				{
					return true;
				}
				else if ( l_hash > r_hash )
				{
					return false;
				}

				if( _l->getPrimitiveType() < _r->getPrimitiveType() )
				{
					return true;
				}
				else if( _l->getPrimitiveType() > _r->getPrimitiveType() )
				{
					return false;
				}

				size_t l_textCount = _l->getTextureCount();
				size_t r_textCount = _r->getTextureCount();

				if( l_textCount < r_textCount )
				{
					return true;
				}
				else if( l_textCount > r_textCount )
				{
					return false;
				}

				for( size_t i = 0; i != l_textCount; ++i )
				{
					if( _l->getTexture( i ) < _r->getTexture( i ) )
					{
						return true;
					}
					else if ( _l->getTexture( i ) > _r->getTexture( i ) )
					{
						return false;							
					}
				}

				if( _l->getStage() < _r->getStage() )
				{
					return true;
				}
				else if( _l->getStage() > _r->getStage() )
				{
					return false;
				}

				return false;
			}
		};

		struct key_getter_type
		{
			const RenderMaterial * operator()( const RenderMaterial * _node ) const
			{
				return _node;
			}
		};

	public:
		RenderMaterial();
		~RenderMaterial();

	public:
		void initialize( size_t _id, EPrimitiveType _primitiveType, size_t _textureCount, const RenderTextureInterfacePtr * _textures, const RenderStage * _stage );
		
	public:
		inline size_t getId() const;
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