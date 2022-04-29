#include "cURLScriptEmbedding.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonDocumentTraceback.h"

#include "PyCURLReceiver.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

#include "pybind/pybind.hpp"
#include "pybind/stl/stl_type_cast.hpp"

#include "curl/curl.h"

namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    cURLScriptEmbedding::cURLScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLScriptEmbedding::~cURLScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID cURLScriptEmbedding::downloadAsset( const String & _url, const String & _login, const String & _password, const ConstString & _fileGroupName, const FilePath & _filePath, int32_t _timeout, const pybind::object & _cb, const pybind::args & _args )
    {
        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getFileGroup( _fileGroupName );

        MENGINE_ASSERTION_MEMORY_PANIC( fileGroup, "invalid get file group '%s' for url '%s' file '%s'"
            , _fileGroupName.c_str()
            , _url.c_str()
            , _filePath.c_str()
        );

        PyCURLReceiverPtr receiver = m_factoryPyHttpReceiver->createObject( MENGINE_DOCUMENT_PYBIND );

        MENGINE_ASSERTION_MEMORY_PANIC( receiver );

        receiver->initialize( _cb, _args );

        uint32_t id = CURL_SERVICE()
            ->downloadAsset( _url, _login, _password, fileGroup, _filePath, _timeout, receiver, MENGINE_DOCUMENT_PYBIND );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID cURLScriptEmbedding::postMessage( const String & _url, const MapParams & _params, int32_t _timeout, const pybind::object & _cb, const pybind::args & _args )
    {
        PyCURLReceiverPtr receiver = m_factoryPyHttpReceiver->createObject( MENGINE_DOCUMENT_PYBIND );

        MENGINE_ASSERTION_MEMORY_PANIC( receiver );

        receiver->initialize( _cb, _args );

        cURLPostParams params;
        for( auto && [key, value] : _params )
        {
            params.emplace_back( cURLPostParam{ key.c_str(), value } );
        }

        HttpRequestID id = CURL_SERVICE()
            ->postMessage( _url, {}, _timeout, false, params, receiver, MENGINE_DOCUMENT_PYBIND );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID cURLScriptEmbedding::headerData( const String & _url, const VectorString & _headers, const String & _data, int32_t _timeout, const pybind::object & _cb, const pybind::args & _args )
    {
        PyCURLReceiverPtr receiver = m_factoryPyHttpReceiver->createObject( MENGINE_DOCUMENT_PYBIND );

        MENGINE_ASSERTION_MEMORY_PANIC( receiver );

        receiver->initialize( _cb, _args );

        HttpRequestID id = CURL_SERVICE()
            ->headerData( _url, _headers, _timeout, false, _data, receiver, MENGINE_DOCUMENT_PYBIND );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID cURLScriptEmbedding::getMessage( const String & _url, int32_t _timeout, const pybind::object & _cb, const pybind::args & _args )
    {
        PyCURLReceiverPtr receiver = m_factoryPyHttpReceiver->createObject( MENGINE_DOCUMENT_PYBIND );

        MENGINE_ASSERTION_MEMORY_PANIC( receiver );

        receiver->initialize( _cb, _args );

        HttpRequestID id = CURL_SERVICE()
            ->getMessage( _url, {}, _timeout, false, receiver, MENGINE_DOCUMENT_PYBIND );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLScriptEmbedding::cancelRequest( HttpRequestID _id )
    {
        CURL_SERVICE()
            ->cancelRequest( _id );
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        SCRIPT_SERVICE()
            ->setAvailablePlugin( "cURL", true );

        pybind::enum_<CURLcode>( _kernel, "CURLcode" )
            .def( "CURLE_OK", CURLE_OK )
            .def( "CURLE_UNSUPPORTED_PROTOCOL", CURLE_UNSUPPORTED_PROTOCOL )
            .def( "CURLE_FAILED_INIT", CURLE_FAILED_INIT )
            .def( "CURLE_URL_MALFORMAT", CURLE_URL_MALFORMAT )
            .def( "CURLE_NOT_BUILT_IN", CURLE_NOT_BUILT_IN )
            .def( "CURLE_COULDNT_RESOLVE_PROXY", CURLE_COULDNT_RESOLVE_PROXY )
            .def( "CURLE_COULDNT_RESOLVE_HOST", CURLE_COULDNT_RESOLVE_HOST )
            .def( "CURLE_COULDNT_CONNECT", CURLE_COULDNT_CONNECT )
            .def( "CURLE_WEIRD_SERVER_REPLY", CURLE_WEIRD_SERVER_REPLY )
            .def( "CURLE_REMOTE_ACCESS_DENIED", CURLE_REMOTE_ACCESS_DENIED )
            .def( "CURLE_FTP_ACCEPT_FAILED", CURLE_FTP_ACCEPT_FAILED )
            .def( "CURLE_FTP_WEIRD_PASS_REPLY", CURLE_FTP_WEIRD_PASS_REPLY )
            .def( "CURLE_FTP_ACCEPT_TIMEOUT", CURLE_FTP_ACCEPT_TIMEOUT )
            .def( "CURLE_FTP_WEIRD_PASV_REPLY", CURLE_FTP_WEIRD_PASV_REPLY )
            .def( "CURLE_FTP_WEIRD_227_FORMAT", CURLE_FTP_WEIRD_227_FORMAT )
            .def( "CURLE_FTP_CANT_GET_HOST", CURLE_FTP_CANT_GET_HOST )
            .def( "CURLE_HTTP2", CURLE_HTTP2 )
            .def( "CURLE_FTP_COULDNT_SET_TYPE", CURLE_FTP_COULDNT_SET_TYPE )
            .def( "CURLE_PARTIAL_FILE", CURLE_PARTIAL_FILE )
            .def( "CURLE_FTP_COULDNT_RETR_FILE", CURLE_FTP_COULDNT_RETR_FILE )
            .def( "CURLE_OBSOLETE20", CURLE_OBSOLETE20 )
            .def( "CURLE_QUOTE_ERROR", CURLE_QUOTE_ERROR )
            .def( "CURLE_HTTP_RETURNED_ERROR", CURLE_HTTP_RETURNED_ERROR )
            .def( "CURLE_WRITE_ERROR", CURLE_WRITE_ERROR )
            .def( "CURLE_OBSOLETE24", CURLE_OBSOLETE24 )
            .def( "CURLE_UPLOAD_FAILED", CURLE_UPLOAD_FAILED )
            .def( "CURLE_READ_ERROR", CURLE_READ_ERROR )
            .def( "CURLE_OUT_OF_MEMORY", CURLE_OUT_OF_MEMORY )
            .def( "CURLE_OPERATION_TIMEDOUT", CURLE_OPERATION_TIMEDOUT )
            .def( "CURLE_OBSOLETE29", CURLE_OBSOLETE29 )
            .def( "CURLE_FTP_PORT_FAILED", CURLE_FTP_PORT_FAILED )
            .def( "CURLE_FTP_COULDNT_USE_REST", CURLE_FTP_COULDNT_USE_REST )
            .def( "CURLE_OBSOLETE32", CURLE_OBSOLETE32 )
            .def( "CURLE_RANGE_ERROR", CURLE_RANGE_ERROR )
            .def( "CURLE_HTTP_POST_ERROR", CURLE_HTTP_POST_ERROR )
            .def( "CURLE_SSL_CONNECT_ERROR", CURLE_SSL_CONNECT_ERROR )
            .def( "CURLE_BAD_DOWNLOAD_RESUME", CURLE_BAD_DOWNLOAD_RESUME )
            .def( "CURLE_FILE_COULDNT_READ_FILE", CURLE_FILE_COULDNT_READ_FILE )
            .def( "CURLE_LDAP_CANNOT_BIND", CURLE_LDAP_CANNOT_BIND )
            .def( "CURLE_LDAP_SEARCH_FAILED", CURLE_LDAP_SEARCH_FAILED )
            .def( "CURLE_OBSOLETE40", CURLE_OBSOLETE40 )
            .def( "CURLE_FUNCTION_NOT_FOUND", CURLE_FUNCTION_NOT_FOUND )
            .def( "CURLE_ABORTED_BY_CALLBACK", CURLE_ABORTED_BY_CALLBACK )
            .def( "CURLE_BAD_FUNCTION_ARGUMENT", CURLE_BAD_FUNCTION_ARGUMENT )
            .def( "CURLE_OBSOLETE44", CURLE_OBSOLETE44 )
            .def( "CURLE_INTERFACE_FAILED", CURLE_INTERFACE_FAILED )
            .def( "CURLE_OBSOLETE46", CURLE_OBSOLETE46 )
            .def( "CURLE_TOO_MANY_REDIRECTS", CURLE_TOO_MANY_REDIRECTS )
            .def( "CURLE_UNKNOWN_OPTION", CURLE_UNKNOWN_OPTION )
            .def( "CURLE_TELNET_OPTION_SYNTAX", CURLE_TELNET_OPTION_SYNTAX )
            .def( "CURLE_OBSOLETE50", CURLE_OBSOLETE50 )
            .def( "CURLE_PEER_FAILED_VERIFICATION", CURLE_PEER_FAILED_VERIFICATION )
            .def( "CURLE_GOT_NOTHING", CURLE_GOT_NOTHING )
            .def( "CURLE_SSL_ENGINE_NOTFOUND", CURLE_SSL_ENGINE_NOTFOUND )
            .def( "CURLE_SSL_ENGINE_SETFAILED", CURLE_SSL_ENGINE_SETFAILED )
            .def( "CURLE_SEND_ERROR", CURLE_SEND_ERROR )
            .def( "CURLE_RECV_ERROR", CURLE_RECV_ERROR )
            .def( "CURLE_OBSOLETE57", CURLE_OBSOLETE57 )
            .def( "CURLE_SSL_CERTPROBLEM", CURLE_SSL_CERTPROBLEM )
            .def( "CURLE_SSL_CIPHER", CURLE_SSL_CIPHER )
            .def( "CURLE_SSL_CACERT", CURLE_SSL_CACERT )
            .def( "CURLE_BAD_CONTENT_ENCODING", CURLE_BAD_CONTENT_ENCODING )
            .def( "CURLE_LDAP_INVALID_URL", CURLE_LDAP_INVALID_URL )
            .def( "CURLE_FILESIZE_EXCEEDED", CURLE_FILESIZE_EXCEEDED )
            .def( "CURLE_USE_SSL_FAILED", CURLE_USE_SSL_FAILED )
            .def( "CURLE_SEND_FAIL_REWIND", CURLE_SEND_FAIL_REWIND )
            .def( "CURLE_SSL_ENGINE_INITFAILED", CURLE_SSL_ENGINE_INITFAILED )
            .def( "CURLE_LOGIN_DENIED", CURLE_LOGIN_DENIED )
            .def( "CURLE_TFTP_NOTFOUND", CURLE_TFTP_NOTFOUND )
            .def( "CURLE_TFTP_PERM", CURLE_TFTP_PERM )
            .def( "CURLE_REMOTE_DISK_FULL", CURLE_REMOTE_DISK_FULL )
            .def( "CURLE_TFTP_ILLEGAL", CURLE_TFTP_ILLEGAL )
            .def( "CURLE_TFTP_UNKNOWNID", CURLE_TFTP_UNKNOWNID )
            .def( "CURLE_REMOTE_FILE_EXISTS", CURLE_REMOTE_FILE_EXISTS )
            .def( "CURLE_TFTP_NOSUCHUSER", CURLE_TFTP_NOSUCHUSER )
            .def( "CURLE_CONV_FAILED", CURLE_CONV_FAILED )
            .def( "CURLE_CONV_REQD", CURLE_CONV_REQD )
            .def( "CURLE_SSL_CACERT_BADFILE", CURLE_SSL_CACERT_BADFILE )
            .def( "CURLE_REMOTE_FILE_NOT_FOUND", CURLE_REMOTE_FILE_NOT_FOUND )
            .def( "CURLE_SSH", CURLE_SSH )
            .def( "CURLE_SSL_SHUTDOWN_FAILED", CURLE_SSL_SHUTDOWN_FAILED )
            .def( "CURLE_AGAIN", CURLE_AGAIN )
            .def( "CURLE_SSL_CRL_BADFILE", CURLE_SSL_CRL_BADFILE )
            .def( "CURLE_SSL_ISSUER_ERROR", CURLE_SSL_ISSUER_ERROR )
            .def( "CURLE_FTP_PRET_FAILED", CURLE_FTP_PRET_FAILED )
            .def( "CURLE_RTSP_CSEQ_ERROR", CURLE_RTSP_CSEQ_ERROR )
            .def( "CURLE_RTSP_SESSION_ERROR", CURLE_RTSP_SESSION_ERROR )
            .def( "CURLE_FTP_BAD_FILE_LIST", CURLE_FTP_BAD_FILE_LIST )
            .def( "CURLE_CHUNK_FAILED", CURLE_CHUNK_FAILED )
            .def( "CURLE_NO_CONNECTION_AVAILABLE", CURLE_NO_CONNECTION_AVAILABLE )
            .def( "CURLE_SSL_PINNEDPUBKEYNOTMATCH", CURLE_SSL_PINNEDPUBKEYNOTMATCH )
            .def( "CURLE_SSL_INVALIDCERTSTATUS", CURLE_SSL_INVALIDCERTSTATUS )
            .def( "CURLE_HTTP2_STREAM", CURLE_HTTP2_STREAM )
            ;

        pybind::def_functor_args( _kernel, "getMessage", this, &cURLScriptEmbedding::getMessage );
        pybind::def_functor_args( _kernel, "postMessage", this, &cURLScriptEmbedding::postMessage );
        pybind::def_functor_args( _kernel, "headerData", this, &cURLScriptEmbedding::headerData );
        pybind::def_functor_args( _kernel, "downloadAsset", this, &cURLScriptEmbedding::downloadAsset );
        pybind::def_functor( _kernel, "cancelRequest", this, &cURLScriptEmbedding::cancelRequest );

        m_factoryPyHttpReceiver = Helper::makeFactoryPool<PyCURLReceiver, 32>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "getMessage", nullptr );
        _kernel->remove_from_module( "postMessage", nullptr );
        _kernel->remove_from_module( "headerData", nullptr );
        _kernel->remove_from_module( "downloadAsset", nullptr );
        _kernel->remove_from_module( "cancelRequest", nullptr );

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPyHttpReceiver );

        m_factoryPyHttpReceiver = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}

