#include "LoaderResourceCursorICO.h"

#include "Engine/ResourceCursorICO.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
	LoaderResourceCursorICO::LoaderResourceCursorICO()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	LoaderResourceCursorICO::~LoaderResourceCursorICO()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceCursorICO::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
		ResourceCursorICOPtr resource = stdex::intrusive_static_cast<ResourceCursorICOPtr>(_loadable);

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceCursorICO * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceCursorICO *>(_meta);

        const FilePath & path = metadata->get_File_Path();
		resource->setFilePath( path );

        return true;
    }    
}