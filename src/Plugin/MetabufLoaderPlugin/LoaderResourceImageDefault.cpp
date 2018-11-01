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
    bool LoaderResourceImageDefault::_loader( const Metabuf::Metadata * _meta )
    {
		ResourceImageDefaultPtr resource = stdex::intrusive_static_cast<ResourceImageDefaultPtr>(_loadable);

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

		ConstString converterType;
        metadata->get_File_Converter( &converterType );
		resource->setConverterType( converterType );

        bool hasAlpha = true;
        metadata->get_File_Alpha( &hasAlpha );
		resource->setAlpha( hasAlpha );

        bool isPremultiply = false;
        metadata->get_File_Premultiply( &isPremultiply );
		resource->setPremultiply( isPremultiply );

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

		mt::vec2f offset;
        metadata->get_File_Offset( &offset );
		resource->setOffset( offset );

        bool validNoExist = false;
        metadata->get_File_NoExist( &validNoExist );
		resource->setValidNoExist( validNoExist );

        return true;
    }
}
