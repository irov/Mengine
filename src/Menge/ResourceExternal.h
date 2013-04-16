#	pragma once

#	include "Kernel/ResourceReference.h"

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
		bool _loader( const Metabuf::Metadata * _parser ) override;
		
	protected:
		bool _compile() override;
		void _release() override;

	protected:
        FilePath m_resourcePath;
        ConstString m_resourceName;

        ResourceReference * m_resourceExternal;
	};
}