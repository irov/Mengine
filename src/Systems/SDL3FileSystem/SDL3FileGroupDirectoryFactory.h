#pragma once

#include "Kernel/FactoryWithMutex.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    class SDL3FileGroupDirectoryFactory
        : public FactoryWithMutex
    {
        DECLARE_FACTORABLE( SDL3FileGroupDirectoryFactory );

    public:
        SDL3FileGroupDirectoryFactory( const FilePath & _relationPath );
        ~SDL3FileGroupDirectoryFactory() override;

    protected:
        Factorable * _createObject() override;
        void _destroyObject( Factorable * _obj ) override;

    protected:
        FilePath m_relationPath;
    };
}