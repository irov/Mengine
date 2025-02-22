#include "ResourceSpineSkeleton.h"

#include "Interface/ResourceServiceInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/ContentHelper.h"

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
    void ResourceSpineSkeleton::setResourceSpineAtlas( const ResourcePtr & _resourceSpineAtlas )
    {
        m_resourceSpineAtlas = Helper::staticResourceCast<ResourceSpineAtlasPtr>( _resourceSpineAtlas );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceSpineAtlasPtr & ResourceSpineSkeleton::getResourceSpineAtlas() const
    {
        return m_resourceSpineAtlas;
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
        if( m_resourceSpineAtlas->compile() == false )
        {
            return false;
        }

        const ContentInterfacePtr & content = this->getContent();

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        MemoryInterfacePtr skeleton_memory = Helper::createMemoryCacheFile( fileGroup, filePath, false, false, MENGINE_DOCUMENT_FACTORABLE );
        
        MENGINE_ASSERTION_MEMORY_PANIC( skeleton_memory, "invalid create memory cache file '%s'"
            , filePath.c_str()
        );

        const char * skeleton_memory_buffer = skeleton_memory->getBuffer();

        spAtlas * atlas = m_resourceSpineAtlas->getSpineAtlas();

        spSkeletonJson * skeletonJson = spSkeletonJson_create( atlas );

        MENGINE_ASSERTION_MEMORY_PANIC( skeletonJson, "invalid create skeleton json" );

        spSkeletonData * skeletonData = spSkeletonJson_readSkeletonData( skeletonJson, skeleton_memory_buffer );
        
        MENGINE_ASSERTION_MEMORY_PANIC( skeletonData, "invalid read skeleton data" );

        skeleton_memory = nullptr;

        spSkeletonJson_dispose( skeletonJson );

        if( skeletonData == nullptr )
        {
            LOGGER_ERROR( "resource spine skeleton '%s' invalid read skeleton data, file path '%s'"
                , this->getName().c_str()
                , Helper::getContentFullPath( content ).c_str()
            );

            return false;
        }

        m_skeletonData = skeletonData;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceSpineSkeleton::_release()
    {
        if( m_skeletonData != nullptr )
        {
            spSkeletonData_dispose( m_skeletonData );
            m_skeletonData = nullptr;
        }

        m_resourceSpineAtlas->release();
    }
    //////////////////////////////////////////////////////////////////////////
}