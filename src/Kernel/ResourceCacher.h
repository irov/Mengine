#pragma once

#include "Kernel/Vector.h"

#include "Config/Algorithm.h"

namespace Mengine
{
    template<class T>
    class ResourceCacher
    {
    public:
        ResourceCacher()
        {
        }

        ~ResourceCacher()
        {
        }

    public:
        void addCache( const T & _ptr );
        void removeCache( const T & _ptr );

    public:
        void lock();
        void unlock();
        void clear();

    public:
        typedef Lambda<bool( const T & )> LambdaFilter;
        const T & findCache( const LambdaFilter & _filter ) const;

    protected:
        struct ResourceCacherDesc
        {
            T value;
            mutable bool use;
            bool lock;
        };

    protected:
        typedef Vector<ResourceCacherDesc> VectorResourceCacherDesc;
        VectorResourceCacherDesc m_cachers;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    void ResourceCacher<T>::addCache( const T & _ptr )
    {
        ResourceCacherDesc desc;
        desc.value = _ptr;
        desc.use = true;
        desc.lock = false;

        m_cachers.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    void ResourceCacher<T>::removeCache( const T & _ptr )
    {
        for( typename VectorResourceCacherDesc::iterator
            it = m_cachers.begin(),
            it_end = m_cachers.end();
            it != it_end;
            ++it )
        {
            ResourceCacherDesc & desc = *it;

            if( desc.value != _ptr )
            {
                continue;
            }

            desc.use = false;

            if( desc.lock == true )
            {
                continue;
            }

            m_cachers.erase( it );
            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    void ResourceCacher<T>::lock()
    {
        for( ResourceCacherDesc & desc : m_cachers )
        {
            desc.lock = true;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    void ResourceCacher<T>::unlock()
    {
        m_cachers.erase( Algorithm::remove_if( m_cachers.begin(), m_cachers.end(), []( ResourceCacher<T>::ResourceCacherDesc & _desc )
        {
            _desc.lock = false;

            if( _desc.use == true )
            {
                return false;
            }

            _desc.use = false;

            _desc.value = nullptr;

            return true;
        } ), m_cachers.end() );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    void ResourceCacher<T>::clear()
    {
        m_cachers.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    const T & ResourceCacher<T>::findCache( const typename ResourceCacher<T>::LambdaFilter & _filter ) const
    {
        for( const ResourceCacherDesc & desc : m_cachers )
        {
            if( desc.use == true )
            {
                continue;
            }

            if( _filter != nullptr && _filter( desc.value ) == false )
            {
                continue;
            }

            desc.use = true;

            return desc.value;
        }

        return T::none();
    }
}
