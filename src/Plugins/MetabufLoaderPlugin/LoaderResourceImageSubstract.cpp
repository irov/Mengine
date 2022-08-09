#include "LoaderResourceImageSubstract.h"

#include "Interface/ResourceBankInterface.h"

#include "Kernel/ResourceImageSubstract.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceImageSubstract::LoaderResourceImageSubstract()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceImageSubstract::~LoaderResourceImageSubstract()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceImageSubstract::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceImageSubstract * resource = stdex::intrusive_get<ResourceImageSubstract *>( _loadable );

        const ConstString & groupName = resource->getGroupName();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSubstract * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSubstract *>(_meta);

        const ConstString & resourceImageName = metadata->get_Image_Name();

        ResourceBankInterface * resourceBank = resource->getResourceBank();

        const ResourceImagePtr & resourceImage = resourceBank->getResource( groupName, resourceImageName );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceImage, "'%s' group '%s' invalid get image resource"
            , resource->getName().c_str()
            , resource->getGroupName().c_str()
        );

        resource->setResourceImage( resourceImage );

        const mt::uv4f & uvImage = metadata->get_Image_UV();

        resource->setUVImage( uvImage );
        resource->setUVAlpha( uvImage );

        bool uvImageRotate = false;
        metadata->get_Image_UVRotate( &uvImageRotate );

        resource->setUVImageRotate( uvImageRotate );
        resource->setUVAlphaRotate( uvImageRotate );

        metadata->getm_Image_Alpha( resource, &ResourceImageSubstract::setAlpha );

        const mt::vec2f & maxSize = metadata->get_Image_MaxSize();
        resource->setMaxSize( maxSize );

        mt::vec2f size;
        if( metadata->get_Image_Size( &size ) == true )
        {
            resource->setSize( size );
        }
        else
        {
            resource->setSize( maxSize );
        }

        metadata->getm_Image_Offset( resource, &ResourceImageSubstract::setOffset );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
