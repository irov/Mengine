#	pragma once

#	include <vector>

#	include "IntrusiveList.h"

namespace Menge
{
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
		typedef TemplateChunk<TSizeType, TChunkSize> TChunk;

		struct Block
		{
			TChunk * chunk;
			char buff[TSizeType];
		};

	public:
		TemplateChunk()
			: current_buffer(0)
			, current_free(0)
		{
			Block ** it_free = free_block;

			for( Block * it = buffer_block, 
				*it_end = buffer_block + TChunkSize; 
			it != it_end; 
			++it )
			{
				it->chunk = this;
				*it_free = it;
				++it_free;
			}

			current_free = TChunkSize;
		}

	public:
		void * alloc()
		{
			Block * mem = free_block[--current_free];

			return mem->buff;
		}

		void free( Block * _block )
		{
			free_block[current_free++] = _block;
		}

		bool empty() const
		{
			return current_free == 0;
		}

		void print()
		{
			for( size_t it = 0; it != current_free; ++it )
			{
				ptrdiff_t diff = free_block[it] - buffer_block;

				printf("%d ", diff );
			}

			printf("\n" );
		}

	protected:
		Block buffer_block[TChunkSize];
		size_t current_buffer;

		Block * free_block[TChunkSize];
		size_t current_free;
	};

	template<size_t TSizeType, size_t TChunkSize>
	class TemplatePool
	{
		typedef TemplateChunk<TSizeType, TChunkSize> TChunk;
		typedef typename TChunk::Block TBlock;

	public:
		TemplatePool()
		{
			addChunk_();
		}

	public:
		void * alloc()
		{
			if( m_current->empty() == true )
			{
				addChunk_();
			}

			return m_current->alloc();
		}

		void free( void * _buff )
		{
			TBlock * block = getBlock_( _buff );
			TChunk * chunk = block->chunk;

			chunk->free( block );

			if( m_current != chunk )
			{
				m_chunks.erase( chunk );
				m_chunks.push_front( chunk );

				m_current = chunk;
			}
		}

		void print()
		{
			for( TListChunks::iterator 
				it = m_chunks.begin(),
				it_end = m_chunks.end();
			it != it_end;
			++it)
			{
				(*it)->print();
			}
		}

	protected:
		void addChunk_()
		{
			TChunk * chunk = new TChunk;

			m_current = chunk;
			m_chunks.push_back( chunk );
		}

		TBlock * getBlock_( void * _buff )
		{
			TBlock * block = (TBlock *)((char*)_buff - offsetof(TBlock, buff));

			return block;
		}

	protected:
		typedef IntrusiveList<TChunk> TListChunks;
		TListChunks m_chunks;

		TChunk * m_current;
	};
}
