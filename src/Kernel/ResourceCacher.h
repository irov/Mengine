#pragma once

#include "stdex/stl_vector.h"

namespace Mengine
{
	template<class T>
	class ResourceCacher
	{
	protected:
		struct ResourceCacherDesc
		{
			T value;
			bool use;
			bool lock;
		};

	public:
		ResourceCacher()
		{
		}

		~ResourceCacher()
		{
		}
		
	public:
		void addCache( const T & _ptr )
		{
			ResourceCacherDesc desc;
			desc.value = _ptr;
			desc.use = true;
			desc.lock = false;

			m_cachers.push_back( desc );
		}

	public:
		void removeCache( const T & _ptr )
		{
			for( typename TVectorResourceCacherDesc::iterator
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

	public:
		void lock()
		{
			for( typename TVectorResourceCacherDesc::iterator
				it = m_cachers.begin(),
				it_end = m_cachers.end();
			it != it_end;
			++it )
			{
				ResourceCacherDesc & desc = *it;

				desc.lock = true;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		class FEraseCacher
		{
		public:
			bool operator () ( ResourceCacherDesc & _desc )
			{
				_desc.lock = false;

				if( _desc.use == true )
				{
					return false;
				}

				_desc.use = false;

				_desc.value = nullptr;

				return true;					 
			}
		};
		
		void unlock()
		{
			m_cachers.erase( 
				std::remove_if( m_cachers.begin(), m_cachers.end(), FEraseCacher() ), m_cachers.end() 
				);
		}

		void clear()
		{
			m_cachers.clear();
		}

	public:
		T findCache()
		{
			for( typename TVectorResourceCacherDesc::iterator
				it = m_cachers.begin(),
				it_end = m_cachers.end();
			it != it_end;
			++it )
			{
				ResourceCacherDesc & desc = *it;

				if( desc.use == true )
				{
					continue;
				}

				desc.use = true;

				return desc.value;
			}

			return nullptr;
		}

	protected:
		typedef stdex::vector<ResourceCacherDesc> TVectorResourceCacherDesc;
		TVectorResourceCacherDesc m_cachers;
	};
}
