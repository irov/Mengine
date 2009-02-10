/*
 *	ResourceTexture.h
 *
 *	Created by _Berserk_ on 9.2.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "ResourceReference.h"

namespace Menge
{
	class ResourceTexture
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceMaterial );
	public:
		ResourceTexture( const ResourceFactoryParam& _params );
		~ResourceTexture();

	public:
		void loader( XmlElement * _xml ) override;
		bool _compile() override;
		void _release() override;

	private:
		
	};
}	// namespace Menge
