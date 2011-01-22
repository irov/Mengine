/*
 *	Material.h
 *
 *	Created by _Berserk_ on 16.2.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/RenderSystemInterface.h"
#	include "Math/mat4.h"

namespace Menge
{
	class Texture;

	struct TextureStage
	{
		ETextureAddressMode addressU;
		ETextureAddressMode addressV;

		ETextureOp colorOp;
		ETextureArgument colorArg1;
		ETextureArgument colorArg2;
		ETextureOp alphaOp;
		ETextureArgument alphaArg1;
		ETextureArgument alphaArg2;

		TextureStage()
			: addressU( TAM_CLAMP )
			, addressV( TAM_CLAMP )
			, colorOp( TOP_DISABLE )
			, colorArg1( TARG_TEXTURE )
			, colorArg2( TARG_DIFFUSE )
			, alphaOp( TOP_DISABLE )
			, alphaArg1( TARG_TEXTURE )
			, alphaArg2( TARG_DIFFUSE )
		{
		}
	};

	struct Material
	{
		EBlendFactor blendSrc;
		EBlendFactor blendDst;

		TextureStage textureStage[MENGE_MAX_TEXTURE_STAGES];

		Material()
			: blendSrc( BF_ONE )
			, blendDst( BF_ZERO )
		{
		}
	};

	struct MaterialGroup
	{
		Material group[4];

		inline const Material * getMaterial( ETextureAddressMode _textureU, ETextureAddressMode _textureV ) const
		{
			return &group[_textureU + (_textureV * 2)];
		}
	};
}	// namespace Menge
