#include "AppleHttpRequestGetAsset.h"

#import "Environment/Apple/AppleString.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/FilePathHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequestGetAsset::AppleHttpRequestGetAsset()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequestGetAsset::~AppleHttpRequestGetAsset()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleHttpRequestGetAsset::setLogin( const String & _login )
    {
        m_login = _login;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & AppleHttpRequestGetAsset::getLogin() const
    {
        return m_login;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleHttpRequestGetAsset::setPassword( const String & _password )
    {
        m_password = _password;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & AppleHttpRequestGetAsset::getPassword() const
    {
        return m_password;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleHttpRequestGetAsset::setContent( const ContentInterfacePtr & _content )
    {
        m_content = _content;
    }
    //////////////////////////////////////////////////////////////////////////
    const ContentInterfacePtr & AppleHttpRequestGetAsset::getContent() const
    {
        return m_content;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleHttpRequestGetAsset::_onThreadTaskRun()
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
            , Helper::getContentFullPath( m_content )
        );

        m_stream = stream;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MengineHttpResponseParam * AppleHttpRequestGetAsset::_onHttp( MengineHttpRequestParam * _request )
    {
        NSString * ns_login = Helper::stringToNSString( m_login );
        NSString * ns_password = Helper::stringToNSString( m_password );
        
        MengineHttpResponseParam * response = [MengineHttpNetwork httpRequestGetAsset:_request login:ns_login password:ns_password];
        
        return response;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleHttpRequestGetAsset::_onThreadTaskComplete( bool _successful )
    {
        bool successful_stream_flush = true;

        FileGroupInterfacePtr fileGroup = m_content->getFileGroup();
        FilePath filePath = m_content->getFilePath();
        m_content = nullptr;

        if( m_stream != nullptr )
        {
            successful_stream_flush = m_stream->flush();

            Helper::closeOutputStreamFile( fileGroup, m_stream );
            m_stream = nullptr;
        }

        EHttpCode code = m_response->getCode();

        if( _successful == false || HTTP_CODE_IS_SUCCESSFUL( code ) == false || successful_stream_flush == false )
        {
            AppleHttpRequest::_onThreadTaskComplete( false );

            return;
        }
        
        FilePath filePathTmp = Helper::stringizeFilePathFormat( "%s.~tmp", filePath.c_str() );

        if( fileGroup->moveFile( filePathTmp, filePath ) == false )
        {
            AppleHttpRequest::_onThreadTaskComplete( false );

            return;
        }

        AppleHttpRequest::_onThreadTaskComplete( true );
    }
    //////////////////////////////////////////////////////////////////////////
}
