#include "ResourceSpineSkeleton.h"

#include "Interface/ResourceServiceInterface.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"

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
    void ResourceSpineSkeleton::setResourceSpineAtlasName( const ConstString & _resourceSpineAtlas )
    {
        m_resourceSpineAtlasName = _resourceSpineAtlas;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceSpineSkeleton::getResourceSpineAtlasName() const
    {
        return m_resourceSpineAtlasName;
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
        ResourceSpineAtlasPtr resourceSpineAtlas = RESOURCE_SERVICE()
            ->getResource( m_resourceSpineAtlasName );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceSpineAtlas, false );

        m_resourceSpineAtlas = resourceSpineAtlas;

        const FilePath & filePath = this->getFilePath();
        const FileGroupInterfacePtr & fileGroup = this->getFileGroup();

        MemoryInterfacePtr skeleton_memory = Helper::createMemoryCacheFile( fileGroup, filePath, false, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( skeleton_memory, false );

        const char * skeleton_memory_buffer = skeleton_memory->getBuffer();

        spAtlas * atlas = m_resourceSpineAtlas->getSpineAtlas();

        spSkeletonJson * skeletonJson = spSkeletonJson_create( atlas );

        spSkeletonData * skeletonData = spSkeletonJson_readSkeletonData( skeletonJson, skeleton_memory_buffer );

        skeleton_memory = nullptr;

        spSkeletonJson_dispose( skeletonJson );

        if( skeletonData == nullptr )
        {
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

        m_resourceSpineAtlas = nullptr;
    }
}