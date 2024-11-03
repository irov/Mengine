#pragma once

#include "Kernel/FactoryWithMutex.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    class AppleFileGroupDirectoryFactory
        : public FactoryWithMutex
    {
        DECLARE_FACTORABLE( AppleFileGroupDirectoryFactory );

    public:
        AppleFileGroupDirectoryFactory( const FilePath & _relationPath );
        ~AppleFileGroupDirectoryFactory() override;

    protected:
        Factorable * _createObject() override;
        void _destroyObject( Factorable * _obj ) override;

    protected:
        FilePath m_relationPath;
    };
}