#	include "ResourceSpineSkeleton.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceSpineSkeleton::ResourceSpineSkeleton()
        : m_skeletonData( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceSpineSkeleton::~ResourceSpineSkeleton()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    spSkeletonData * ResourceSpineSkeleton::getSkeletonData() const
    {
        return m_skeletonData;
    }
    //////////////////////////////////////////////////////////////////////////
    spAnimation * ResourceSpineSkeleton::findSkeletonAnimation( const ConstString & _name ) const
    {
        spAnimation * animation = spSkeletonData_findAnimation( m_skeletonData, _name.c_str() );

        return animation;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceSpineSkeleton::_compile()
    {
        const FilePath & filePath = this->getFilePath();
        const FileGroupInterfacePtr & fileGroup = this->getFileGroup();

        MemoryInterfacePtr skeleton_memory = Helper::createMemoryCacheFile( fileGroup, filePath, false, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( skeleton_memory, false );

        const char * skeleton_buffer = skeleton_memory->getBuffer();

        spSkeletonJson * skeletonJson = spSkeletonJson_create( atlas );

        spSkeletonData * skeletonData = spSkeletonJson_readSkeletonData( skeletonJson, skeleton_buffer );

        skeleton_buffer = nullptr;

        spSkeletonJson_dispose( skeletonJson );

        if( skeletonData == nullptr )
        {
            spAtlas_dispose( atlas );

            for( TVectorImageDesc::iterator
                it = m_images.begin(),
                it_end = m_images.end();
                it != it_end;
                ++it )
            {
                const ResourceImagePtr & image = it->image;

                image->decrementReference();
            }

            return false;
        }

        m_atlas = atlas;
        m_skeletonData = skeletonData;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceSpine::_release()
    {
        for( TVectorImageDesc::iterator
            it = m_images.begin(),
            it_end = m_images.end();
            it != it_end;
            ++it )
        {
            const ResourceImagePtr & image = it->image;

            image->decrementReference();
        }

        if( m_skeletonData != nullptr )
        {
            spSkeletonData_dispose( m_skeletonData );
            m_skeletonData = nullptr;
        }

        if( m_atlas != nullptr )
        {
            spAtlas_dispose( m_atlas );
            m_atlas = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceSpine::_isValid() const
    {
        return true;
    }
}