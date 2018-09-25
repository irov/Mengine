#include "ResourceOzzAnimation.h"

#include "Interface/FileSystemInterface.h"

#include "Kernel/MemoryHelper.h"

#include "ozz/base/io/stream.h"
#include "ozz/base/io/archive.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceOzzAnimation::ResourceOzzAnimation()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceOzzAnimation::~ResourceOzzAnimation()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceOzzAnimation::setFilePath( const FilePath & _filePath )
    {
        m_filePath = _filePath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & ResourceOzzAnimation::getFilePath() const
    {
        return m_filePath;
    }
    //////////////////////////////////////////////////////////////////////////
    const ozz::animation::Animation & ResourceOzzAnimation::getOzzAnimation() const
    {
        return m_animation;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceOzzAnimation::_compile()
    {
        const FileGroupInterfacePtr & category = this->getFileGroup();

        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile( category, m_filePath, false );

        if( stream == nullptr )
        {
            return false;
        }

        MemoryInterfacePtr memory = Helper::createMemoryStream( stream, "ResourceOzzAnimation", __FILE__, __LINE__ );

        if( memory == nullptr )
        {
            return false;
        }

        const void * memory_buffer = memory->getBuffer();
        size_t memory_size = memory->getSize();

        ozz::io::MemoryStream ozz_stream;
        ozz_stream.Write( memory_buffer, memory_size );
        ozz_stream.Seek( 0, ozz::io::Stream::kSet );

        ozz::io::IArchive ozz_archive( &ozz_stream );

        ozz_archive >> m_animation;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceOzzAnimation::_release()
    {
        //Empty
    }
}