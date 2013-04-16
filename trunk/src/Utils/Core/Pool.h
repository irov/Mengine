#	pragma once

#	include "Core/IntrusiveList.h"
#	include "Core/IntrusiveSlug.h"

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
            : m_free(0)
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
			if( m_free == 0 )
			{
                this->addChunk_();
			}

			TBlock * free = m_free;
			m_free = m_free->next;
			return free;
		}

		void free( void * _buff )
		{
			TBlock * block = reinterpret_cast<TBlock*>(_buff);
			block->next = m_free;
			m_free = block;
		}

	protected:
		void addChunk_()
		{
			TChunk * chunk = new TChunk( &m_free );

			m_chunks.push_back( chunk );
		}

	protected:
		TListChunks m_chunks;

		TBlock * m_free;
	};
}
