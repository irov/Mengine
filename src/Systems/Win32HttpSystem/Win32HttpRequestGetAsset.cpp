#include "Win32HttpRequestGetAsset.h"

#include "Win32HttpNetwork.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/StreamHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32HttpRequestGetAsset::Win32HttpRequestGetAsset()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32HttpRequestGetAsset::~Win32HttpRequestGetAsset()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32HttpRequestGetAsset::setLogin( const String & _login )
    {
        m_login = _login;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & Win32HttpRequestGetAsset::getLogin() const
    {
        return m_login;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32HttpRequestGetAsset::setPassword( const String & _password )
    {
        m_password = _password;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & Win32HttpRequestGetAsset::getPassword() const
    {
        return m_password;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32HttpRequestGetAsset::setContent( const ContentInterfacePtr & _content )
    {
        m_content = _content;
    }
    //////////////////////////////////////////////////////////////////////////
    const ContentInterfacePtr & Win32HttpRequestGetAsset::getContent() const
    {
        return m_content;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32HttpRequestGetAsset::_onThreadTaskRun()
    {
        if( m_content->createDirectory() == false )
        {
            return false;
        }

        OutputStreamInterfacePtr stream = m_content->openOutputStreamFile( true, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "get asset url '%s' invalid open file '%s'"
            , m_url.c_str()
            , Helper::getContentFullPath( m_content ).c_str()
        );

        m_stream = stream;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32HttpRequestGetAsset::_onHttp()
    {
        bool successful = Helper::httpRequestGetAsset( this, m_response, m_login, m_password );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32HttpRequestGetAsset::_onThreadTaskComplete( bool _successful )
    {
        EHttpCode code = m_response->getCode();

        ContentInterfacePtr content = std::move( m_content );
        OutputStreamInterfacePtr stream = std::move( m_stream );

        if( _successful == false )
        {
            LOGGER_MESSAGE( "get asset url '%s' failed http request code '%d'"
                , m_url.c_str()
                , code
            );

            Win32HttpRequest::_onThreadTaskComplete( false );

            return;
        }

        if( HTTP_CODE_IS_SUCCESSFUL( code ) == false )
        {
            LOGGER_MESSAGE( "get asset url '%s' invalid http request code '%d'"
                , m_url.c_str()
                , code
            );

            Win32HttpRequest::_onThreadTaskComplete( false );

            return;
        }

        if( stream != nullptr )
        {
            const HttpResponseInterfacePtr & response = this->getReponse();

            const Data & data = response->getData();

            Helper::writeStreamData( stream, data );

            bool successful_stream_flush = stream->flush();

            bool successful_stream_close = content->closeOutputStreamFile( stream );
            m_stream = nullptr;

            if( successful_stream_flush == false )
            {
                LOGGER_WARNING( "get asset url '%s' invalid flush stream for file '%s'"
                    , m_url.c_str()
                    , Helper::getContentFullPath( content ).c_str()
                );

                Win32HttpRequest::_onThreadTaskComplete( false );

                return;
            }

            if( successful_stream_close == false )
            {
                LOGGER_WARNING( "get asset url '%s' invalid close stream for file '%s'"
                    , m_url.c_str()
                    , Helper::getContentFullPath( content ).c_str()
                );

                Win32HttpRequest::_onThreadTaskComplete( false );

                return;
            }
        }

        Win32HttpRequest::_onThreadTaskComplete( true );
    }
    //////////////////////////////////////////////////////////////////////////
}
