#	pragma once

namespace Menge
{
	template<size_t TSizeType, size_t TChunkSize>
	class TemplateChunk
	{
    public:
        typedef TemplateChunk<TSizeType, TChunkSize> TChunk;

	public:
		struct Block
		{
			char buff[TSizeType];
			Block * next;
		};

	public:
		TemplateChunk( Block ** _free, TChunk * _prev )
            : prev(_prev)
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
        TChunk * getPrev() const
        {
            return prev;
        }

    protected:
		Block buffer_block[TChunkSize];
        TemplateChunk<TSizeType, TChunkSize> * prev;
	};

	template<size_t TSizeType, size_t TChunkSize>
	class Pool
	{
		typedef TemplateChunk<TSizeType, TChunkSize> TChunk;
		typedef typename TChunk::Block TBlock;

    public:
        Pool()
            : m_chunk(nullptr)
            , m_free(nullptr)
            , m_countBlock(0)
            , m_countChunk(0)
        {
        }

        ~Pool()
        {
            TChunk * chunk = m_chunk;

            while( chunk != nullptr )
            {
                TChunk * prev = chunk->getPrev();

                delete chunk;

                chunk = prev;
            }
        }

	public:
		void * alloc()
		{
			if( m_free == nullptr )
			{
                this->addChunk_();
			}

			TBlock * free = m_free;
			m_free = m_free->next;

            ++m_countBlock;

            void * impl = static_cast<void *>(free);

			return impl;
		}

		void free( void * _buff )
		{
			TBlock * block = reinterpret_cast<TBlock*>(_buff);
            
			block->next = m_free;

			m_free = block;
            --m_countBlock;
		}

        bool empty() const
        {
            return m_countBlock == 0;
        }

	protected:
		void addChunk_()
		{
			TChunk * chunk = new TChunk(&m_free, m_chunk);

            m_chunk = chunk;

            ++m_countChunk;
		}

	protected:
		TChunk * m_chunk;
		TBlock * m_free;

        size_t m_countBlock;
        size_t m_countChunk;
	};

    template<class T, size_t TChunkSize>
    class TemplatePool
    {
    public:
        TemplatePool()
        {
        }

    public:
        T * createT()
        {
            void * impl = m_pool.alloc();

            T * t = new (impl) T;
            
            return t;
        }

        void destroyT( T * _t )
        {
            _t->~T();

            m_pool.free( _t );
        }

    public:
        bool empty() const
        {
            bool result = m_pool.empty();

            return result;
        }

    protected:
        typedef Pool<sizeof(T), TChunkSize> TPool;
        TPool m_pool;
    };
}
