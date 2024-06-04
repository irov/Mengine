#pragma once

#include "Win32HttpRequest.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32HttpRequestHeaderData
        : public Win32HttpRequest
    {
        DECLARE_FACTORABLE( Win32HttpRequestHeaderData );

    public:
        Win32HttpRequestHeaderData();
        ~Win32HttpRequestHeaderData() override;

    public:
        void setData( const Data & _data );
        const Data & getData() const;

    protected:
        bool _onHttp() override;

    protected:
        Data m_data;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32HttpRequestHeaderData, ThreadTaskInterface> Win32HttpRequestHeaderDataPtr;
    //////////////////////////////////////////////////////////////////////////
}
