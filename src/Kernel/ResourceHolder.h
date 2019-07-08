#pragma once

#include "Kernel/Resource.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class ResourceHolder
    {
    public:
        typedef IntrusiveResourcePtr<T> TPtr;

    public:
        ResourceHolder()
            : m_resource( nullptr )
            , m_compile( false )
        {
        }

        ResourceHolder( const TPtr & _resource )
            : m_resource( _resource )
            , m_compile( false )
        {
        }

    public:
        const TPtr & get() const
        {
            return m_resource;
        }

        bool empty() const
        {
            return m_resource == nullptr;
        }

    public:
        ResourceHolder<T> & operator = ( const TPtr & _resource )
        {
            this->release();

            m_resource = _resource;

            return *this;
        }

    public:
        const TPtr & operator -> () const
        {
            const TPtr & t = this->get();

            return t;
        }

        operator const ResourcePtr & () const
        {
            const ResourcePtr & t = this->get();

            return t;
        }

        operator const TPtr & () const
        {
            const TPtr & t = this->get();

            return t;
        }

        bool operator == ( const TPtr & _resource ) const
        {
            const TPtr & t = this->get();

            bool result = (t == _resource);

            return result;
        }

        bool operator != ( const TPtr & _resource ) const
        {
            return !this->operator == ( _resource );
        }

    public:
        bool compile()
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

        void release()
        {
            if( m_compile == true )
            {
                m_resource->decrementReference();
            }

            m_compile = false;
        }

        bool isCompile() const
        {
            return m_compile;
        }

    protected:
        TPtr m_resource;
        bool m_compile;
    };
}