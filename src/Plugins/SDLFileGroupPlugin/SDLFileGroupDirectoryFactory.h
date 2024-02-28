#pragma once

#include "Kernel/FactoryWithoutMutex.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    class SDLFileGroupDirectoryFactory
        : public FactoryWithoutMutex
    {
        DECLARE_FACTORABLE( SDLFileGroupDirectoryFactory );

    public:
        SDLFileGroupDirectoryFactory( const FilePath & _relationPath );
        ~SDLFileGroupDirectoryFactory() override;

    protected:
        Factorable * _createObject() override;
        void _destroyObject( Factorable * _obj ) override;

    protected:
        FilePath m_relationPath;
    };
}