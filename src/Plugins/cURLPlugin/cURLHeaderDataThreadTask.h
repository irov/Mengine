#pragma once

#include "cURLThreadTask.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class cURLHeaderDataThreadTask
        : public cURLThreadTask
    {
        DECLARE_FACTORABLE( cURLHeaderDataThreadTask );

    public:
        cURLHeaderDataThreadTask();
        ~cURLHeaderDataThreadTask() override;

    public:
        bool initialize( const String & _data );

    protected:
        void _onCURL( CURL * _curl ) override;
        void _onCURLCleanup( CURL * _curl ) override;

    protected:
        String m_data;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLHeaderDataThreadTask, ThreadTaskInterface> cURLHeaderDataThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}