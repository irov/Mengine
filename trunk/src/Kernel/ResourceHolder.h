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

    public:
        bool empty() const;

    protected:
        void setReference( ResourceReference * _resource );

    protected:
        ResourceReference * m_reference;
        ResourceReference * m_resource;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class ResourceHolder
        : public ResourceHolderBase
    {
    public:
        T * get() const
        {
#   ifdef _DEBUG
            if( dynamic_cast<T *>(m_resource) == nullptr )
            {
                return nullptr;
            }
#   endif

            T * t = static_cast<T *>(m_resource);

            return t;
        }

        T * reference() const
        {
#   ifdef _DEBUG
            if( dynamic_cast<T *>(m_reference) == nullptr )
            {
                return nullptr;
            }
#   endif

            T * t = static_cast<T *>(m_reference);

            return t;
        }

    public:
        ResourceHolder<T> & operator = ( T * _resource )
        {
            this->setReference( _resource );

            return *this;
        }
        
    public:
        T * operator -> () const
        {
            T * t = this->get();

            return t;
        }

        //operator T * () const
        //{
        //    T * t = this->get();

        //    return t;
        //}

        bool operator == ( T * _resource ) const
        {
            T * t = this->reference();

            bool result = (t == _resource);

            return result;
        }

        bool operator != ( T * _resource ) const
        {
            T * t = this->reference();

            bool result = (t != _resource);

            return result;
        }
    };
}