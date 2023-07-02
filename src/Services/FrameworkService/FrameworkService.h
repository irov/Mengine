#pragma once

#include "Interface/FrameworkServiceInterface.h"
#include "Interface/FrameworkInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Hashtable.h"
#include "Kernel/ConstString.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    class FrameworkService
        : public ServiceBase<FrameworkServiceInterface>
    {
    public:
        FrameworkService();
        ~FrameworkService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool hasFramework( const ConstString & _name ) const override;
        bool initializeFramework( const ConstString & _name, const DocumentInterfacePtr & _doc ) override;
        bool finalizeFramework( const ConstString & _name ) override;
        bool isInitializeFramework( const ConstString & _name ) const override;

    public:
        bool runFrameworks() override;
        bool stopFrameworks() override;

    protected:
        const FrameworkInterfacePtr & findFramework_( const ConstString & _moduleName ) const;
        FrameworkInterfacePtr popFramework_( const ConstString & _moduleName );

    protected:
        typedef Vector<FrameworkInterfacePtr> VectorFrameworks;
        VectorFrameworks m_frameworks;
    };
}