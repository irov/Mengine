#include "Movie2Data.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"

#include "Kernel/Logger.h"

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
        ae_delete_movie_data( m_movieData );
        m_movieData = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2Data::acquire()
    {
        for( const ResourcePtr & resource : m_resources )
        {
            if( resource->incrementReference() == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Data::release()
    {
        for( const ResourcePtr & resource : m_resources )
        {
            resource->decrementReference();
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
    Resource * Movie2Data::getResource( const ae_string_t _resourceName )
    {
        const ResourcePtr & resource = RESOURCE_SERVICE()
            ->getResourceReference( Helper::stringizeString( _resourceName ) );

        if( resource == nullptr )
        {
            LOGGER_ERROR( "not found resource '%s'"
                , _resourceName
            );

            return nullptr;
        }

        m_resources.emplace_back( resource );

        return resource.get();
    }
}