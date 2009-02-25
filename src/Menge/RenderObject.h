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
#	include "ColourValue.h"

namespace Menge
{
	class ResourceTexture;
	class ResourceImage;

	struct TextureStage
	{
		ResourceImage* image;
		std::size_t image_frame;
		ETextureAddressMode addressU;
		ETextureAddressMode addressV;

		TextureStage()
			: image( NULL )
			, image_frame( 0 )
			, addressU( TAM_CLAMP )
			, addressV( TAM_CLAMP )
		{
		}
	};

	struct RenderPass
	{
		EPrimitiveType primitiveType;
		std::vector<uint16> indicies;	
		std::vector<uint16> batchedIndicies;

		EBlendFactor blendSrc;
		EBlendFactor blendDst;
		ColourValue color;

		std::size_t textureStages;
		TextureStage textureStage[MENGE_MAX_TEXTURE_STAGES];

		// private info
		std::size_t startIndex;
		std::size_t indiciesNum;

		RenderPass()
			: primitiveType( PT_POINTLIST )
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
				|| textureStages != _other.textureStages 
				|| color != _other.color )
			{
				return false;
			}
			for( std::size_t i = 0; i < textureStages; ++i )
			{
				if( textureStage[i].image != _other.textureStage[i].image
					|| textureStage[i].image_frame != _other.textureStage[i].image_frame
					|| textureStage[i].addressU != _other.textureStage[i].addressU
					|| textureStage[i].addressV != _other.textureStage[i].addressV )
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
		std::vector<RenderPass> passes;

		bool setWorldTransform;
		mt::mat4f worldTransform;

		bool setViewTransform;
		mt::mat4f viewTransform; 

		bool setProjTransform;
		mt::mat4f projTransform; 

		bool setRenderArea;
		mt::vec4f renderArea;

		bool setRenderTarget;
		String renderTargetName;

		// private info
		bool batched;
		std::size_t vertexIndex;
		std::size_t verticesNum;
		VBHandle vbHandle;
		IBHandle ibHandle;

		RenderObject()
			: setWorldTransform( false )
			, setViewTransform( false )
			, setProjTransform( false )
			, setRenderArea( false )
			, setRenderTarget( false )
			, batched( false )
			, vertexIndex( 0 )
			, verticesNum( 0 )
			, vbHandle( 0 )
			, ibHandle( 0 )
		{
		}

		bool operator==( RenderObject* _other )
		{
			if( passes.size() != _other->passes.size()
				|| vbHandle != _other->vbHandle
				|| ibHandle != _other->ibHandle
				|| setWorldTransform != false || _other->setWorldTransform != false
				|| setViewTransform != false || _other->setViewTransform != false
				|| setProjTransform != false || _other->setProjTransform != false
				|| setRenderArea != false || _other->setRenderArea != false 
				|| setRenderTarget != false || _other->setRenderTarget != false )
			{
				return false;
			}
			for( std::size_t i = 0; i < passes.size(); ++i )
			{
				if( passes[i].operator==( _other->passes[i] ) == false )
				{
					return false;
				}
			}
			return true;
		}
	};

}	// namespace Menge
