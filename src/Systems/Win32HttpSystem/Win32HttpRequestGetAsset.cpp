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
        const FileGroupInterfacePtr & fileGroup = m_content->getFileGroup();
        const FilePath & filePath = m_content->getFilePath();

        if( fileGroup->createDirectory( filePath ) == false )
        {
            return false;
        }
        
        FilePath filePathTmp = Helper::stringizeFilePathFormat( "%s.~tmp", filePath.c_str() );

        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( fileGroup, filePathTmp, true, MENGINE_DOCUMENT_FACTORABLE );

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
        bool successful_stream_flush = true;

        FileGroupInterfacePtr fileGroup = m_content->getFileGroup();
        FilePath filePath = m_content->getFilePath();
        m_content = nullptr;

        if( m_stream != nullptr )
        {
            const HttpResponseInterfacePtr & response = this->getReponse();

            const Data & data = response->getData();

            Helper::writeStreamData( m_stream, data );

            successful_stream_flush = m_stream->flush();

            Helper::closeOutputStreamFile( fileGroup, m_stream );
            m_stream = nullptr;
        }

        EHttpCode code = m_response->getCode();

        if( _successful == false || HTTP_CODE_IS_SUCCESSFUL( code ) == false || successful_stream_flush == false )
        {
            Win32HttpRequest::_onThreadTaskComplete( false );

            return;
        }
        
        FilePath filePathTmp = Helper::stringizeFilePathFormat( "%s.~tmp", filePath.c_str() );

        if( fileGroup->moveFile( filePathTmp, filePath ) == false )
        {
            Win32HttpRequest::_onThreadTaskComplete( false );

            return;
        }

        Win32HttpRequest::_onThreadTaskComplete( true );
    }
    //////////////////////////////////////////////////////////////////////////
}
