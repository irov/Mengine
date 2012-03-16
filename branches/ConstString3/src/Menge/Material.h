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
	
	struct TextureStage
	{
		ETextureAddressMode addressU;
		ETextureAddressMode addressV;

		ETextureFilter filter;

		ETextureOp colorOp;
		ETextureArgument colorArg1;
		ETextureArgument colorArg2;
		ETextureOp alphaOp;
		ETextureArgument alphaArg1;
		ETextureArgument alphaArg2;

		TextureStage()
			: addressU(TAM_CLAMP)
			, addressV(TAM_CLAMP)
			, filter(TF_LINEAR)
			, colorOp(TOP_DISABLE)
			, colorArg1(TARG_TEXTURE)
			, colorArg2(TARG_DIFFUSE)
			, alphaOp(TOP_DISABLE)
			, alphaArg1(TARG_TEXTURE)
			, alphaArg2(TARG_DIFFUSE)
		{
		}
	};

	struct Material
	{
		Material()
			: blendSrc(BF_ONE)
			, blendDst(BF_ZERO)
			, depthBufferWriteEnable(false)
			, alphaTestEnable(false)
			, alphaBlendEnable(false)
		{
		}

		TextureStage textureStage[MENGE_MAX_TEXTURE_STAGES];

		EBlendFactor blendSrc;
		EBlendFactor blendDst;

		bool depthBufferWriteEnable;
		bool alphaTestEnable;
		bool alphaBlendEnable;

		const Material * filterMaterial;

		const Material * getFilterMaterial() const
		{
			return filterMaterial;
		}
	};

	struct MaterialGroup
	{
		Material group[4];
		Material filter_group[4];

		inline const Material * getMaterial( ETextureAddressMode _textureU, ETextureAddressMode _textureV) const
		{
			return &group[_textureU + (_textureV * 2)];
		}
	};
}	// namespace Menge
