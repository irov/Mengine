#	include "ResourceInternalObject.h"
#	include "Kernel/ResourceImplement.h"

#	include "Metacode.h"

#	include "Interface/ScriptSystemInterface.h"

#	include "Kernel/Scriptable.h"
#	include "Kernel/Node.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceInternalObject );
	//////////////////////////////////////////////////////////////////////////
	ResourceInternalObject::ResourceInternalObject()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceInternalObject::~ResourceInternalObject()		
	{

	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceInternalObject::getInternalName() const
	{
		return m_internalName;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceInternalObject::getInternalGroup() const
	{
		return m_internalGroup;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceInternalObject::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceInternalObject * metadata
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceInternalObject *>(_meta);

        metadata->swap_Internal_Name( m_internalName );
        metadata->swap_Internal_Group( m_internalGroup );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceInternalObject::_compile()
	{
		if( m_internalName.empty() == true )
		{
			return false;
		}

		if( m_internalGroup.empty() == true )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceInternalObject::_release()
	{
	}
}
