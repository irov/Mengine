#	pragma once

namespace Menge
{
	template<size_t TSizeType, size_t TChunkCount>
	class TemplateChunk
	{
    public:
        typedef TemplateChunk<TSizeType, TChunkCount> TChunk;

	public:
		struct Block
		{
			char buff[TSizeType];
            TChunk * chunk;
			Block * next;
		};

	public:
		TemplateChunk()
            : m_prev(nullptr)
            , m_next(nullptr)
            , m_freeBlock(nullptr)
            , m_countBlock(TChunkCount)
		{
            Block * block = nullptr;

			for( Block * it = buffer_block, 
				*it_end = buffer_block + TChunkCount; 
				it != it_end; 
			++it )
			{
                it->chunk = this;
				it->next = block;
				block = it;
			}

            m_freeBlock = block;
		}

        Block * alloc()
        {
            Block * block = m_freeBlock;
            m_freeBlock = m_freeBlock->next;

            --m_countBlock;

            return block;
        }

        void free( Block * _block )
        {
            _block->next = m_freeBlock;
            m_freeBlock = _block;

            ++m_countBlock;
        }

    public:
        void unlink()
        {
            if( m_prev != nullptr )
            {
                m_prev->m_next = m_next;
            }

            if( m_next != nullptr )
            {
                m_next->m_prev = m_prev;
            }

            m_prev = nullptr;
            m_next = nullptr;
        }

        void push_front( TChunk * _chunk )
        {
            m_prev = nullptr;
            m_next = _chunk;

            _chunk->m_prev = this;
        }

    protected:
		Block buffer_block[TChunkCount];

    public:
        TChunk * m_prev;
        TChunk * m_next;

        Block * m_freeBlock;

        size_t m_countBlock;
	};

	template<size_t TSizeType, size_t TChunkCount>
	class Pool
	{
		typedef TemplateChunk<TSizeType, TChunkCount> TChunk;
		typedef typename TChunk::Block TBlock;

    public:
        Pool()
            : m_freeChunk(nullptr)
            , m_fullChunk(nullptr)
            , m_emptyChunk(nullptr)
            , m_countBlock(0)
            , m_countBlockMax(0)
            , m_countChunk(0)
        {
        }

        ~Pool()
        {
            TChunk * chunk = m_freeChunk;

            while( chunk != nullptr )
            {
                TChunk * next = chunk->m_next;

                delete chunk;

                chunk = next;
            }

            chunk = m_fullChunk;

            while( chunk != nullptr )
            {
                TChunk * next = chunk->m_next;

                delete chunk;

                chunk = next;
            }

            chunk = m_emptyChunk;

            while( chunk != nullptr )
            {
                TChunk * next = chunk->m_next;

                delete chunk;

                chunk = next;
            }
        }

	public:
		void * alloc()
		{
			if( m_freeChunk == nullptr )
			{
                this->addChunk_();
			}

			TBlock * free = m_freeChunk->alloc();

            ++m_countBlock;

            this->updateAllockChunks_();
            
            void * impl = static_cast<void *>(free);

			return impl;
		}

		void free( void * _buff )
		{
			TBlock * block = reinterpret_cast<TBlock*>(_buff);
            
            TChunk * chunk = block->chunk;
			chunk->free( block );
                        
            --m_countBlock;

            this->updateFreeChunks_( chunk );
		}

        bool empty() const
        {
            return m_countBlock == 0;
        }

	protected:
        void updateAllockChunks_()
        {
            if( m_freeChunk->m_countBlock == 0 )
            {
                TChunk * free = m_freeChunk;
                m_freeChunk = m_freeChunk->m_next;

                free->unlink();

                if( m_fullChunk != nullptr )
                {
                    free->push_front( m_fullChunk );
                }

                m_fullChunk = free;
            }
        }

        void updateFreeChunks_( TChunk * _chunk )
        {
            const size_t chunkCount = TChunkCount;

            if( _chunk->m_countBlock == chunkCount )
            {
                if( m_freeChunk == _chunk )
                {
                    m_freeChunk = m_freeChunk->m_next;
                }
                else if( m_fullChunk == _chunk )
                {
                    m_fullChunk = m_fullChunk->m_next;
                }

                _chunk->unlink();

                if( m_emptyChunk != nullptr )
                {
                    _chunk->push_front( m_emptyChunk );
                }

                m_emptyChunk = _chunk;
            }

            this->updateEmptyChunk();
        }

        void updateEmptyChunk()
        {
            if( m_emptyChunk == nullptr )
            {
                return;
            }

            const size_t chunkCount2 = TChunkCount * 2;

            if( m_countBlockMax - m_countBlock > chunkCount2 )
            {
                TChunk * free = m_emptyChunk;
                m_emptyChunk = m_emptyChunk->m_next;

                free->unlink();

                delete free;

                --m_countChunk;
                m_countBlockMax -= TChunkCount;
            }
        }

		void addChunk_()
		{
            TChunk * chunk = nullptr;

            if( m_emptyChunk != nullptr )
            {
                chunk = m_emptyChunk;
                m_emptyChunk = m_emptyChunk->m_next;

                chunk->unlink();
            }
            else
            {
			    chunk = new TChunk();

                ++m_countChunk;

                const size_t chunkCount = TChunkCount;

                m_countBlockMax += chunkCount;
            }

            if( m_freeChunk != nullptr )
            {
                chunk->push_front( m_freeChunk );
            }

            m_freeChunk = chunk;
		}

	protected:
		TChunk * m_freeChunk;
        TChunk * m_fullChunk;
        TChunk * m_emptyChunk;

        size_t m_countBlock;
        size_t m_countBlockMax;
        size_t m_countChunk;
	};

    template<class T, size_t TChunkCount>
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
        typedef Pool<sizeof(T), TChunkCount> TPool;
        TPool m_pool;
    };
}
