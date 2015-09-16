#   pragma once

#   include "ResourceReference.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class ResourceHolder
    {
	public:
		ResourceHolder()
			: m_resource(nullptr)
			, m_compile(false)
		{
		}

		ResourceHolder( T * _resource )
			: m_resource(_resource)
			, m_compile(false)
		{
		}

    public:
        T * get() const
        {
            return m_resource;
        }

    public:
        ResourceHolder<T> & operator = ( T * _resource )
        {
			this->release();

			m_resource = _resource;
			
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

		inline bool isCompile() const
		{
			return m_compile;
		}

	protected:
		T * m_resource;
		bool m_compile;
    };
}