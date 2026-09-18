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
#include "Kernel/StreamHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequestGetAsset::AppleHttpRequestGetAsset()
        : AppleHttpRequest( false )
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
        
        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( fileGroup, filePath, true, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "get asset url '%s' invalid open file '%s'"
            , m_url.c_str()
            , Helper::getContentFullPath( m_content ).c_str()
        );

        m_stream = stream;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    AppleHttpResponseParam * AppleHttpRequestGetAsset::_onHttp( AppleHttpRequestParam * _request )
    {
        NSString * ns_login = [AppleString NSStringFromString:m_login];
        NSString * ns_password = [AppleString NSStringFromString:m_password];
        
        AppleHttpResponseParam * response = [AppleHttpNetwork httpRequestGetAsset:_request login:ns_login password:ns_password];
        
        return response;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleHttpRequestGetAsset::_onThreadTaskComplete( bool _successful )
    {
        EHttpCode code = m_response->getCode();

        bool successful_asset = _successful;

        if( HTTP_CODE_IS_SUCCESSFUL( code ) == false )
        {
            successful_asset = false;
        }

        FileGroupInterfacePtr fileGroup = m_content->getFileGroup();
        m_content = nullptr;

        if( m_stream != nullptr )
        {
            if( successful_asset == true )
            {
                const HttpResponseInterfacePtr & response = this->getReponse();

                const Data & data = response->getData();

                Helper::writeStreamData( m_stream, data );

                if( m_stream->flush() == false )
                {
                    successful_asset = false;
                }
            }

            if( Helper::closeOutputStreamFile( fileGroup, m_stream, successful_asset ) == false )
            {
                successful_asset = false;
            }

            m_stream = nullptr;
        }

        AppleHttpRequest::_onThreadTaskComplete( successful_asset );
    }
    //////////////////////////////////////////////////////////////////////////
}
