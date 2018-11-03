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
		ResourceCursorICO * resource = stdex::intrusive_get<ResourceCursorICO *>(_loadable);

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceCursorICO * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceCursorICO *>(_meta);

        metadata->getm_File_Path( resource, &ResourceCursorICO::setFilePath );

        return true;
    }    
}