#	pragma once

#	include "Core/IntrusiveList.h"
#	include "Core/IntrusiveSlug.h"

#   ifdef _DEBUG
#   include <vector>
#   include <algorithm>

typedef std::vector<void *> TVectorAllockBlock;
#   endif

namespace Menge
{
	template<size_t TSizeType, size_t TChunkSize>
	class TemplateChunk
		: public IntrusiveLinked<TemplateChunk<TSizeType,TChunkSize> >
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

        typedef IntrusiveList<TChunk> TListChunks;
        typedef IntrusiveSlug<TChunk> TSlugChunks;

    public:
        TemplatePool()
            : m_free(nullptr)
            , m_count(0)
        {
        }

        ~TemplatePool()
        {
            for( TSlugChunks it(m_chunks); it.eof() == false; it.next_shuffle() )
            {
                TChunk * chunk = *it;

                delete chunk;
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

            ++m_count;

            void * impl = static_cast<void *>(free);
#   ifdef _DEBUG
            m_allockBlock.push_back( impl );
#   endif

			return impl;
		}

		void free( void * _buff )
		{
			TBlock * block = reinterpret_cast<TBlock*>(_buff);

#   ifdef _DEBUG
            TVectorAllockBlock::iterator if_found = std::find( m_allockBlock.begin(), m_allockBlock.end(), _buff );
            m_allockBlock.erase( if_found );
#   endif
            --m_count;

			block->next = m_free;

			m_free = block;
		}

#   ifdef _DEBUG
        TVectorAllockBlock & allock_blocks()
        {
            return m_allockBlock;
        }
#   endif

        bool empty() const
        {
            return m_count == 0;
        }

	protected:
		void addChunk_()
		{
			TChunk * chunk = new TChunk( &m_free );

			m_chunks.push_back( chunk );
		}

	protected:
		TListChunks m_chunks;

#   ifdef _DEBUG
        TVectorAllockBlock m_allockBlock;
#   endif

		TBlock * m_free;

        size_t m_count;
	};
}
