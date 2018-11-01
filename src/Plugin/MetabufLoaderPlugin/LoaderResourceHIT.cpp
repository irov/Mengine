#include "LoaderResourceFile.h"

#include "Engine/ResourceHIT.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
	LoaderResourceFile::LoaderResourceFile()
    {
    }
    //////////////////////////////////////////////////////////////////////////
	LoaderResourceFile::~LoaderResourceFile()
    {
    }    
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceFile::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
		ResourceHITPtr resource = stdex::intrusive_static_cast<ResourceHITPtr>(_loadable);

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceHIT * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceHIT *>(_meta);

        const FilePath & filePath = metadata->get_File_Path();

		resource->setFilePath( filePath );
        
		ConstString converterType;
		metadata->get_File_Converter( &converterType );
		resource->setConverterType( converterType );

		ConstString codecType;
		metadata->get_File_Codec( &codecType );
		resource->setCodecType( codecType );

        return true;
    }    
}
