#   pragma once

namespace Menge
{
    class ResourceReference;

    class ResourceHolderBase
    {
    protected:
        ResourceHolderBase();

    public:        
        bool compile();
        void release();

    protected:
        void setResource( ResourceReference * _resource );
        
        inline ResourceReference * getResource() const
        {
            return m_resource;
        }

    protected:
        ResourceReference * m_resource;
        bool m_compile;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class ResourceHolder
        : public ResourceHolderBase
    {
    public:
        T * get() const
        {
            ResourceReference * resource = this->getResource();

#   ifdef _DEBUG
            if( dynamic_cast<T *>(resource) == nullptr )
            {
                return nullptr;
            }
#   endif

            T * t = static_cast<T *>(resource);

            return t;
        }

    public:
        ResourceHolder<T> & operator = ( T * _resource )
        {
            this->setResource( _resource );

            return *this;
        }
        
    public:
        T * operator -> () const
        {
            T * t = this->get();

            return t;
        }

        operator T * () const
        {
            T * t = this->get();

            return t;
        }

        bool operator == ( T * _resource ) const
        {
            T * t = this->get();

            bool result = (t == _resource);

            return result;
        }

        bool operator != ( T * _resource ) const
        {
            T * t = this->get();

            bool result = (t != _resource);

            return result;
        }
    };
}