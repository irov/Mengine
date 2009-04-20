/*
 *	RenderObject.h
 *
 *	Created by _Berserk_ on 16.2.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	define MENGE_MAX_TEXTURE_STAGES 8

#	include "Interface/RenderSystemInterface.h"

namespace Menge
{
	class Texture;

	struct TextureStage
	{
		Texture* texture;
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
			: texture( NULL )
			, addressU( TAM_CLAMP )
			, addressV( TAM_CLAMP )
			, colorOp( TOP_MODULATE )
			, colorArg1( TARG_TEXTURE )
			, colorArg2( TARG_DIFFUSE )
			, alphaOp( TOP_MODULATE )
			, alphaArg1( TARG_TEXTURE )
			, alphaArg2( TARG_DIFFUSE )
			, matrix( NULL )
		{
		}
	};

	struct RenderPass
	{
		EPrimitiveType primitiveType;
		std::vector<uint16> indicies;	
		std::vector<uint16> batchedIndicies;

		bool isSolidColor;
		EBlendFactor blendSrc;
		EBlendFactor blendDst;

		std::size_t textureStages;
		TextureStage textureStage[MENGE_MAX_TEXTURE_STAGES];

		// private info
		std::size_t startIndex;
		std::size_t indiciesNum;

		RenderPass()
			: primitiveType( PT_POINTLIST )
			, isSolidColor( true )
			, blendSrc( BF_ONE )
			, blendDst( BF_ZERO )
			, textureStages( 0 )
			, startIndex( 0 )
			, indiciesNum( 0 )
		{
		}

		bool operator==( const RenderPass& _other )
		{
			if( primitiveType != _other.primitiveType 
				|| blendSrc != _other.blendSrc
				|| blendDst != _other.blendDst
				|| textureStages != _other.textureStages )
			{
				return false;
			}
			for( std::size_t i = 0; i < textureStages; ++i )
			{
				if( textureStage[i].texture != _other.textureStage[i].texture
					|| textureStage[i].addressU != _other.textureStage[i].addressU
					|| textureStage[i].addressV != _other.textureStage[i].addressV
					|| textureStage[i].colorOp != _other.textureStage[i].colorOp
					|| textureStage[i].colorArg1 != _other.textureStage[i].colorArg1
					|| textureStage[i].colorArg2 != _other.textureStage[i].colorArg2
					|| textureStage[i].alphaOp != _other.textureStage[i].alphaOp
					|| textureStage[i].alphaArg1 != _other.textureStage[i].alphaArg1
					|| textureStage[i].alphaArg2 != _other.textureStage[i].alphaArg2 )
				{
					return false;
				}
			}
			return true;
		}
	};

	class RenderObject
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
			/*for( std::size_t i = 0; i < passes.size(); ++i )
			{
				if( passes[i].operator==( _other->passes[i] ) == false )
				{
					return false;
				}
			}*/
			//return true;
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
	};

}	// namespace Menge
