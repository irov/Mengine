#	pragma once

#	include "ResourceReference.h"

namespace Menge
{
	class ResourceExternal
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceExternal )

	public:
		ResourceExternal();
		~ResourceExternal();

    public:
        ResourceReference * getResourceExternal() const;

	protected:
		void loader( const Metabuf::Metadata * _parser ) override;
		
	protected:
		bool _compile() override;
		void _release() override;

	protected:
        WString m_resourcePath;
        ConstString m_resourceName;

        ResourceReference * m_resourceExternal;
	};
}