#pragma once

#include "Plugins/ResourceValidatePlugin/ResourceValidatorHelper.h"

#include "Interface/ArchivatorInterface.h"

#include "ResourceMovie2.h"

namespace Mengine
{
    class ResourceMovie2Validator
        : public ResourceValidatorT<ResourceMovie2Ptr>
    {
    public:
        ResourceMovie2Validator();
        ~ResourceMovie2Validator() override;

    public:
        void setMovieInstance( const aeMovieInstance * _movieInstance );
        const aeMovieInstance * getMovieInstance() const;

    public:
        void setArchivator( const ArchivatorInterfacePtr & _archivator );
        const ArchivatorInterfacePtr & getArchivator() const;

    public:
        bool _validate( const ResourceMovie2Ptr & _resource ) const override;

    protected:
        const aeMovieInstance * m_movieInstance;

        ArchivatorInterfacePtr m_archivator;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourceMovie2Validator> ResourceMovie2ValidatorPtr;
    //////////////////////////////////////////////////////////////////////////
}