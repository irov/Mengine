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
    bool ResourceHolderBase::compile()
    {
        if( this->empty() == true )
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
    bool ResourceHolderBase::empty() const
    {
        return m_resource == nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceHolderBase::setResource( ResourceReference * _resource )
    {
        this->release();

        m_resource = _resource;
    }
}