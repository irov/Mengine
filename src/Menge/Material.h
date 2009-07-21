/*
 *	Material.h
 *
 *	Created by _Berserk_ on 16.2.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	define MENGE_MAX_TEXTURE_STAGES 8

#	include "Interface/RenderSystemInterface.h"
#	include "Math/mat4.h"

namespace Menge
{
	class Texture;

	struct TextureStage
	{
		//Texture* texture;
		ETextureAddressMode addressU;
		ETextureAddressMode addressV;

		ETextureOp colorOp;
		ETextureArgument colorArg1;
		ETextureArgument colorArg2;
		ETextureOp alphaOp;
		ETextureArgument alphaArg1;
		ETextureArgument alphaArg2;

		mt::mat4f* matrix;

		TextureStage()
			//: texture( NULL )
			: addressU( TAM_CLAMP )
			, addressV( TAM_CLAMP )
			, colorOp( TOP_DISABLE )
			, colorArg1( TARG_TEXTURE )
			, colorArg2( TARG_DIFFUSE )
			, alphaOp( TOP_DISABLE )
			, alphaArg1( TARG_TEXTURE )
			, alphaArg2( TARG_DIFFUSE )
			, matrix( NULL )
		{
		}
	};

	struct Material
	{
		bool isSolidColor;
		EBlendFactor blendSrc;
		EBlendFactor blendDst;

		//std::size_t textureStages;
		TextureStage textureStage[MENGE_MAX_TEXTURE_STAGES];

		Material()
			: isSolidColor( true )
			, blendSrc( BF_ONE )
			, blendDst( BF_ZERO )
			//, textureStages( 0 )
		{
		}

	};

	/*class RenderObject2D
	{
	public:
		EPrimitive2D primitive2DType;
		std::vector<TVertex> vertices;
		RenderPass material;

		// private info
		std::size_t startIndex;

		class TextureSortPredicate
		{
		public:
			bool operator()( RenderObject* const& _obj1, RenderObject* const& _obj2 )
			{
				return _obj1->material.textureStage[0].texture > _obj2->material.textureStage[0].texture;
			}
		};

		class ApplyZ
		{
		public:
			ApplyZ( float _z )
				: m_z( _z )
			{
			}
			void operator()( TVertex& _vtx )
			{
				_vtx.pos[2] = m_z;
			}
		protected:
			float m_z;
		};

	};*/

	/*class RenderObject
	{
	public:
		std::vector<TVertex> vertices;
		RenderPass material;
		//ColourValue color;
		//std::vector<RenderPass> passes;

		bool setWorldTransform;
		mt::mat4f worldTransform;

		// private info
		bool batched;
		std::size_t vertexIndex;
		std::size_t verticesNum;
		VBHandle vbHandle;
		IBHandle ibHandle;

		RenderObject()
			: setWorldTransform( false )
			, batched( false )
			, vertexIndex( 0 )
			, verticesNum( 0 )
			, vbHandle( 0 )
			, ibHandle( 0 )
		{
		}

		bool operator==( RenderObject* _other )
		{
			if( vbHandle != _other->vbHandle
				|| ibHandle != _other->ibHandle
				|| setWorldTransform != false || _other->setWorldTransform != false )
			{
				return false;
			}
			return material.operator ==( _other->material );
		}

		class ApplyColor
		{
		public:
			ApplyColor( uint32 _argb )
				: m_argb( _argb )
			{
			}
			void operator()( TVertex& _vtx )
			{
				_vtx.color = m_argb;
			}

		protected:
			uint32 m_argb;
		};

		class TextureSortPredicate
		{
		public:
			bool operator()( RenderObject* const& _obj1, RenderObject* const& _obj2 )
			{
				return _obj1->material.textureStage[0].texture > _obj2->material.textureStage[0].texture;
			}
		};

		class ApplyZ
		{
		public:
			ApplyZ( float _z )
				: m_z( _z )
			{
			}
			void operator()( TVertex& _vtx )
			{
				_vtx.pos[2] = m_z;
			}
		protected:
			float m_z;
		};
	};*/

}	// namespace Menge
