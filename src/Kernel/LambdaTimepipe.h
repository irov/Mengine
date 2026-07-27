#pragma once

#include "Interface/TimepipeInterface.h"

#include "Kernel/Factorable.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class LambdaTimepipe
        : public Factorable
        , public TimepipeInterface
    {
        DECLARE_FACTORABLE( LambdaTimepipe );

    public:
        typedef Lambda<void( const UpdateContext * )> LambdaEvent;

    public:
        LambdaTimepipe();
        ~LambdaTimepipe() override;

    public:
        void initialize( const LambdaEvent & _lambda );

    protected:
        void onTimepipe( const UpdateContext * _context ) override;

    protected:
        LambdaEvent m_lambda;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<LambdaTimepipe> LambdaTimepipePtr;
    //////////////////////////////////////////////////////////////////////////
}
