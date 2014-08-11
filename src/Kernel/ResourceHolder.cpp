#   include "ResourceHolder.h"

#   include "ResourceReference.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    ResourceHolderBase::ResourceHolderBase()
        : m_resource(nullptr)
        , m_compile(false)
    {
    }
	//////////////////////////////////////////////////////////////////////////
	ResourceHolderBase::ResourceHolderBase( ResourceReference * _resource )
		: m_resource(_resource)
		, m_compile(false)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    bool ResourceHolderBase::compile()
    {
        if( m_resource == nullptr )
        {
            return false;
        }

        if( m_resource->incrementReference() == 0 )
        {
            return false;
        }

        m_compile = true;

        return true;             
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceHolderBase::release()
    {
        if( m_compile == true )
        {
            m_resource->decrementReference();
        }

        m_compile = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceHolderBase::setResource( ResourceReference * _resource )
    {
        this->release();

        m_resource = _resource;
    }
}