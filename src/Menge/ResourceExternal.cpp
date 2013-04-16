#	include "ResourceExternal.h"
#	include "Kernel/ResourceImplement.h"

#	include "Metacode.h"

#	include "ResourceManager.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceExternal );
	//////////////////////////////////////////////////////////////////////////
	ResourceExternal::ResourceExternal()
		: m_resourceExternal(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceExternal::~ResourceExternal()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceReference * ResourceExternal::getResourceExternal() const
	{
		return m_resourceExternal;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceExternal::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceExternal * metadata
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceExternal *>(_meta);

        metadata->swap_External_Name( m_resourceName );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceExternal::_compile()
	{
        m_resourceExternal = RESOURCE_SERVICE(m_serviceProvider)
            ->getResourceT<ResourceReference>( m_resourceName );

        if( m_resourceExternal == NULL )
        {
            return false;
        }
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceExternal::_release()
	{        
        m_resourceExternal->decrementReference();
        m_resourceExternal = NULL;
	}	
	//////////////////////////////////////////////////////////////////////////
}