#include "Movie2Data.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/MaterialEnumInterface.h"

#include "Kernel/Materialable.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Movie2Data::Movie2Data()
        : m_movieData( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Movie2Data::~Movie2Data()
    {
        if( m_movieData != nullptr )
        {
            ae_delete_movie_data( m_movieData );
            m_movieData = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2Data::acquire()
    {
        for( const ResourcePtr & resource : m_resources )
        {
            if( resource->compile() == false )
            {
                return false;
            }
        }

        for( Movie2DataImageDesc * desc : m_images )
        {
            desc->materials[EMB_NORMAL] = Helper::makeImageMaterial( desc->resourceImage, ConstString::none(), EMB_NORMAL, false, false, MENGINE_DOCUMENT_FACTORABLE );
            desc->materials[EMB_ADD] = Helper::makeImageMaterial( desc->resourceImage, ConstString::none(), EMB_ADD, false, false, MENGINE_DOCUMENT_FACTORABLE );
            desc->materials[EMB_SCREEN] = Helper::makeImageMaterial( desc->resourceImage, ConstString::none(), EMB_SCREEN, false, false, MENGINE_DOCUMENT_FACTORABLE );
            desc->materials[EMB_MULTIPLY] = Helper::makeImageMaterial( desc->resourceImage, ConstString::none(), EMB_MULTIPLY, false, false, MENGINE_DOCUMENT_FACTORABLE );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Data::release()
    {
        for( const ResourcePtr & resource : m_resources )
        {
            resource->release();
        }

        for( Movie2DataImageDesc * desc : m_images )
        {
            desc->materials[EMB_NORMAL] = nullptr;
            desc->materials[EMB_ADD] = nullptr;
            desc->materials[EMB_SCREEN] = nullptr;
            desc->materials[EMB_MULTIPLY] = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer Movie2Data::allocateMemory( size_t _size ) const
    {
        MENGINE_UNUSED( _size );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Data::setMovieData( const aeMovieData * _movieData )
    {
        m_movieData = _movieData;
    }
    //////////////////////////////////////////////////////////////////////////
    const aeMovieData * Movie2Data::getMovieData() const
    {
        return m_movieData;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & Movie2Data::getResource( const ae_string_t _resourceName )
    {
        const ResourcePtr & resource = RESOURCE_SERVICE()
            ->getResourceReference( Helper::stringizeString( _resourceName ) );

        MENGINE_ASSERTION_MEMORY_PANIC( resource, "not found resource '%s'"
            , _resourceName
        );

        m_resources.emplace_back( resource );

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    Movie2DataImageDesc * Movie2Data::makeImageDesc( const ResourceImagePtr & _resource )
    {
        Movie2DataImageDesc * desc = m_poolImageDesc.createT();

        desc->resourceImage = _resource;

        m_images.push_back( desc );

        return desc;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Data::removeImageDesc( Movie2DataImageDesc * _desc )
    {
        m_poolImageDesc.destroyT( _desc );
    }
}