#include "LoaderResourceCursorSystem.h"

#include "Engine/ResourceCursorSystem.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
	LoaderResourceCursorSystem::LoaderResourceCursorSystem()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	LoaderResourceCursorSystem::~LoaderResourceCursorSystem()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceCursorSystem::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
		ResourceCursorSystemPtr resource = stdex::intrusive_static_cast<ResourceCursorSystemPtr>(_loadable);

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceCursorSystem * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceCursorSystem *>(_meta);

        const FilePath & filePath = metadata->get_File_Path();

		resource->setFilePath( filePath );

        return true;
    }    
}