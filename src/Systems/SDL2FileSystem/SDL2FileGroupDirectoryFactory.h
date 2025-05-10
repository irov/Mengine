#pragma once

#include "Kernel/FactoryWithMutex.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    class SDL2FileGroupDirectoryFactory
        : public FactoryWithMutex
    {
        DECLARE_FACTORABLE( SDL2FileGroupDirectoryFactory );

    public:
        SDL2FileGroupDirectoryFactory( const FilePath & _relationPath );
        ~SDL2FileGroupDirectoryFactory() override;

    protected:
        Factorable * _createObject() override;
        void _destroyObject( Factorable * _obj ) override;

    protected:
        FilePath m_relationPath;
    };
}