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
	
	struct RenderTextureStage
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

		RenderTextureStage()
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

	struct RenderMaterial
	{
		RenderMaterial()
			: blendSrc(BF_ONE)
			, blendDst(BF_ZERO)
			, depthBufferWriteEnable(false)
			, alphaTestEnable(false)
			, alphaBlendEnable(false)
		{
		}

		RenderTextureStage textureStage[MENGE_MAX_TEXTURE_STAGES];

		EBlendFactor blendSrc;
		EBlendFactor blendDst;

		bool depthBufferWriteEnable;
		bool alphaTestEnable;
		bool alphaBlendEnable;

		const RenderMaterial * filterMaterial;

		const RenderMaterial * getFilterMaterial() const
		{
			return filterMaterial;
		}
	};

	struct RenderMaterialGroup
	{
		RenderMaterial group[4];
		RenderMaterial filter_group[4];

		inline const RenderMaterial * getMaterial( ETextureAddressMode _textureU, ETextureAddressMode _textureV) const
		{
			return &group[_textureU + (_textureV * 2)];
		}
	};
}	// namespace Menge
