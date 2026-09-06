#pragma once

#include "Interface/FindPython3ServiceInterface.h"

#include "Kernel/ServiceBase.h"

#include "Config/Path.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MacOSFindPython3Service
        : public ServiceBase<FindPython3ServiceInterface>
    {
        DECLARE_FACTORABLE( MacOSFindPython3Service );

    public:
        MacOSFindPython3Service();
        ~MacOSFindPython3Service() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        void getPython3ExecutablePath( Char * const _path ) const override;

    protected:
        bool findPython3_();
        bool tryPython3_( const Char * _path, const Char * _source );

    protected:
        Path m_python3ExecutablePath = {'\0'};
    };
    //////////////////////////////////////////////////////////////////////////
}
