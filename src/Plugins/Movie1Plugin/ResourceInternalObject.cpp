#include "ResourceInternalObject.h"

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
    void ResourceInternalObject::setInternalName( const ConstString & _internalName )
    {
        m_internalName = _internalName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceInternalObject::getInternalName() const
    {
        return m_internalName;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceInternalObject::setInternalGroup( const ConstString & _internalGroup )
    {
        m_internalGroup = _internalGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceInternalObject::getInternalGroup() const
    {
        return m_internalGroup;
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
    //////////////////////////////////////////////////////////////////////////
}
