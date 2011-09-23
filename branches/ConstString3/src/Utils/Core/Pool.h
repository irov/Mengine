#	pragma once

#	include <vector>

#	include "IntrusiveList.h"

namespace Menge
{
	template<class T>
	class PoolVector
	{
	public:
		void initialize( std::size_t _maxCount )
		{
			m_values.resize(_maxCount);

			for( typename TVectorIndices::value_type 
				it = 0,
				it_end = m_values.size();
			it != it_end;
			++it )
			{
				m_indices.push_back(it);
			}
		}

	public:
		T * get()
		{
			if( m_indices.empty() == true )
			{
				return 0;
			}

			typename TVectorIndices::value_type index = m_indices.back();
			m_indices.pop_back();

			return &m_values[index];
		}

		void release( T * _value )
		{
			typename TVectorIndices::value_type index = std::distance( &*m_values.begin(), _value );
			m_indices.push_back( index );
		}

	protected:
		typedef std::vector<T> TVectorValues;
		typedef std::vector<typename TVectorValues::size_type> TVectorIndices;

		TVectorValues m_values;
		TVectorIndices m_indices;
	};

	template<class T> 
	class PoolPlacementPolicyNone
	{
	public:
		static void placement( T * _element )
		{
		}
	};

	template<class T>
	class PoolPlacementPolicyErase
	{
	public:
		static void placement( T * _element )
		{
			_element->~T();
			new (_element) T;
		}
	};

	template<class T, template <typename U> class PlacementPolicy = PoolPlacementPolicyErase >
	class Pool
	{
		typedef std::vector<T*> TContainerPool;
		typedef PlacementPolicy<T> TPlacementPolicy;

	public:
		Pool()
		{
		}

		~Pool()
		{
			for( typename TContainerPool::iterator 
				it = m_pool.begin(), 
				it_end = m_pool.end();
			it != it_end;
			++it )
			{
				delete (*it);
			}
		}

	public:
		T * get()
		{
			if( m_pool.empty() == false )
			{
				T * element = m_pool.back();
				m_pool.pop_back();
				return element;
			}

			return new T;
		}

		void release( T * _element )
		{
			if( _element == 0 )
			{
				return;
			}

			TPlacementPolicy::placement( _element );

			m_pool.push_back( _element );
		}

	protected:
		TContainerPool m_pool;
	};

	template<size_t TSizeType, size_t TChunkSize>
	class TemplateChunk
		: public IntrusiveLinked
	{
	public:
		struct Block
		{
			char buff[TSizeType];
			Block * next;
		};

	public:
		TemplateChunk( Block ** _free )
		{
			for( Block * it = buffer_block, 
				*it_end = buffer_block + TChunkSize; 
				it != it_end; 
			++it )
			{
				it->next = *_free;
				*_free = it;
			}
		}

	public:
		Block buffer_block[TChunkSize];
	};

	template<size_t TSizeType, size_t TChunkSize>
	class TemplatePool
	{
		typedef TemplateChunk<TSizeType, TChunkSize> TChunk;
		typedef typename TChunk::Block TBlock;

	public:
		static void * alloc( size_t _count )
		{
			if( m_free == 0 )
			{
				addChunk_();
			}

			TBlock * free = m_free;
			m_free = m_free->next;
			return free;
		}

		static void free( void * _buff, size_t _count )
		{
			TBlock * block = reinterpret_cast<TBlock*>(_buff);
			block->next = m_free;
			m_free = block;
		}

	protected:
		static void addChunk_()
		{
			TChunk * chunk = new TChunk( &m_free );

			m_chunks.push_back( chunk );
		}

	protected:
		typedef IntrusiveList<TChunk> TListChunks;
		static TListChunks m_chunks;

		static TBlock * m_free;
	};

	template<size_t TSizeType, size_t TChunkSize>
	typename TemplatePool<TSizeType, TChunkSize>::TListChunks TemplatePool<TSizeType, TChunkSize>::m_chunks;

	template<size_t TSizeType, size_t TChunkSize>
	typename TemplatePool<TSizeType, TChunkSize>::TBlock * TemplatePool<TSizeType, TChunkSize>::m_free = 0;


	template<size_t TSizeType>
	class TemplatePoolNew
	{
	public:
		static void * alloc( size_t _count )
		{
			return new char[TSizeType * _count];
		}

		static void free( void * _buff, size_t _count )
		{
			delete[] (char*)_buff;
		}
	};

	template <bool, class T1, class T2>
	struct SelectType
	{
		typedef T1 type;
	};

	template <class T1, class T2>
	struct SelectType<false, T1, T2>
	{
		typedef T2 type;
	};

	template <size_t TSizeType, size_t TMaxSizeType, size_t TChunkSize>
	struct TemplatePoolSelect
	{
		typedef typename TemplateChunk<TSizeType, TChunkSize>::Block TBlock;

		typedef typename SelectType<
			(TSizeType < TMaxSizeType),
			TemplatePool< (TSizeType > sizeof(TBlock) ? TSizeType: sizeof(TBlock)) , TChunkSize>,
			TemplatePoolNew<TSizeType>
		>::type type;
	};

	template<class T, size_t TMaxSizeType, size_t TChunkSize>
	class TemplatePoolAllocator
	{
	public:
		typedef T *pointer;
		typedef const T *const_pointer;
		typedef ptrdiff_t difference_type;
		typedef T &reference;
		typedef const T &const_reference;
		typedef size_t size_type;
		typedef T value_type;

		typedef typename TemplatePoolSelect<sizeof(T), TMaxSizeType, TChunkSize>::type TPool;

		template <class U>
		TemplatePoolAllocator(const TemplatePoolAllocator<U, TMaxSizeType, TChunkSize> &in_Other) {};

		TemplatePoolAllocator() {}

		pointer address(reference in_X) const { return &in_X; }
		const_pointer address(const_reference in_X) const { return &in_X; }
		size_type max_size() const { return size_t(-1)/sizeof(T); }

		pointer allocate( size_type _count, void * = 0) 
		{
			return (T*)TPool::alloc( _count );
		}

		void deallocate( pointer _pointer, size_type _count )
		{
			TPool::free(_pointer, _count);
		}

		void construct( pointer _pointer, const value_type &in_Val )
		{
			new (_pointer) T(in_Val);
		}    

		void destroy( pointer _pointer )
		{
			_pointer->~T();
		}

		template <class U>
		struct rebind
		{
			typedef TemplatePoolAllocator<U, TMaxSizeType, TChunkSize> other;
		};
	};
}
