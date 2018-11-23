#pragma once

#include "Plugins/ResourceValidatePlugin/ResourceValidatorHelper.h"

#include "Interface/ArchiveInterface.h"

#include "ResourceMovie2.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class ResourceMovie2Validator
        : public ResourceValidatorT<ResourceMovie2Ptr>
        , public Factorable
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
        bool _validate( const ResourceMovie2Ptr & _resource ) override;

    protected:
        const aeMovieInstance * m_movieInstance;

        ArchivatorInterfacePtr m_archivator;
    };
}