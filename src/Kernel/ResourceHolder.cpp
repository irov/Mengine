#   include "ResourceHolder.h"

#   include "ResourceReference.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    ResourceHolderBase::ResourceHolderBase()
        : m_reference(nullptr)
        , m_resource(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceHolderBase::compile()
    {
        m_resource = m_reference;

        if( m_resource == nullptr )
        {
            return false;
        }

        if( m_resource->incrementReference() == 0 )
        {
            return false;
        }

        return true;             
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceHolderBase::release()
    {
        if( m_resource != nullptr )
        {
            m_resource->decrementReference();
            m_resource = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceHolderBase::empty() const
    {
        return m_reference == nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceHolderBase::setReference( ResourceReference * _resource )
    {
        this->release();

        m_reference = _resource;
    }
}