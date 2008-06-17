#	pragma once

#	include "ResourceReference.h"

namespace Menge
{
	class ResourceMeshMD3
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceMeshMD3 )
	public:
		ResourceMeshMD3( const ResourceFactoryParam & _params );
	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		std::string m_filename;
	};
}	// namespace Menge