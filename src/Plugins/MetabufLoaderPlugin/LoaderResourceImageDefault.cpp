#include "LoaderResourceImageDefault.h"

#include "Interface/CodecServiceInterface.h"

#include "Engine/ResourceImageDefault.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
	LoaderResourceImageDefault::LoaderResourceImageDefault()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	LoaderResourceImageDefault::~LoaderResourceImageDefault()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceImageDefault::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
		ResourceImageDefault * resource = stdex::intrusive_get<ResourceImageDefault *>(_loadable);

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageDefault * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageDefault *>(_meta);

        const FilePath & filePath = metadata->get_File_Path();

		resource->setFilePath( filePath );

		ConstString codecType;
        if( metadata->get_File_Codec( &codecType ) == false )
        {
			codecType = CODEC_SERVICE()
                ->findCodecType( filePath );
        }

		resource->setCodecType( codecType );

        metadata->getm_File_Converter( resource, &ResourceImageDefault::setConverterType );
        metadata->getm_File_Alpha( resource, &ResourceImageDefault::setAlpha );
        metadata->getm_File_Premultiply( resource, &ResourceImageDefault::setPremultiply );

        const mt::vec2f & maxSize = metadata->get_File_MaxSize();
		resource->setMaxSize( maxSize );

		mt::vec2f size;
		if( metadata->get_File_Size( &size ) == true )
		{
			resource->setSize( size );
		}
		else
		{
			resource->setSize( maxSize );
		}
        
        metadata->getm_File_Offset( resource, &ResourceImageDefault::setOffset );
        metadata->getm_File_NoExist( resource, &ResourceImageDefault::setValidNoExist );

        resource->correctUVTexture();

        return true;
    }
}
