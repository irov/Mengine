#pragma once

#include "Interface/DocumentServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/FactoryWithMutex.h"

namespace Mengine
{
    class DocumentService
        : public ServiceBase<DocumentServiceInterface>
    {
    public:
        DocumentService();
        ~DocumentService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        DocumentPtr createDocument( const DocumentPtr & _parent, const Char * _modulePath, const Char * _file, const Char * _function, uint32_t _line, MENGINE_CHECK_FORMAT_STRING( const Char * _format ), ... ) override;

    protected:
        FactoryWithMutexPtr m_factoryDocument;
    };
}