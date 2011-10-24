#	include "ResourceInternalObject.h"
#	include "ResourceImplement.h"

#	include "BinParser.h"

#	include "ScriptEngine.h"

#	include "Scriptable.h"
#	include "Node.h"

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
	void ResourceInternalObject::loader( BinParser * _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::Internal_Name, m_internalName );
			BIN_CASE_ATTRIBUTE( Protocol::Internal_Group, m_internalGroup );
		}
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
