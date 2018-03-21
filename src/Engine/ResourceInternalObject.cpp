#include "ResourceInternalObject.h"

#include "Metacode/Metacode.h"

#include "Kernel/Scriptable.h"
#include "Kernel/Node.h"

namespace Mengine
{
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
        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceInternalObject * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceInternalObject *>(_meta);

        m_internalName = metadata->get_Internal_Name();
        m_internalGroup = metadata->get_Internal_Group();

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
