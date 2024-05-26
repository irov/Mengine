#include "HttpServiceScriptEmbedding.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/HttpServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocumentTraceback.h"

#include "PyHttpReceiver.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/HttpCode.h"
#include "Kernel/ContentHelper.h"

namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    HttpServiceScriptEmbedding::HttpServiceScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    HttpServiceScriptEmbedding::~HttpServiceScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestId HttpServiceScriptEmbedding::downloadAsset( const String & _url, const String & _login, const String & _password, const ConstString & _fileGroupName, const FilePath & _filePath, int32_t _timeout, const pybind::object & _cb, const pybind::args & _args )
    {
        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getFileGroup( _fileGroupName );

        MENGINE_ASSERTION_MEMORY_PANIC( fileGroup, "invalid get file group '%s' for url '%s' file '%s'"
            , _fileGroupName.c_str()
            , _url.c_str()
            , _filePath.c_str()
        );

        PyHttpReceiverPtr receiver = m_factoryPyHttpReceiver->createObject( MENGINE_DOCUMENT_PYBIND );

        MENGINE_ASSERTION_MEMORY_PANIC( receiver );

        receiver->initialize( _cb, _args );

        ContentInterfacePtr content = Helper::makeFileContent( fileGroup, _filePath, MENGINE_DOCUMENT_PYBIND );

        MENGINE_ASSERTION_MEMORY_PANIC( content );

        uint32_t id = HTTP_SERVICE()
            ->downloadAsset( _url, _login, _password, content, _timeout, receiver, MENGINE_DOCUMENT_PYBIND );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestId HttpServiceScriptEmbedding::postMessage( const String & _url, const Params & _params, int32_t _timeout, const pybind::object & _cb, const pybind::args & _args )
    {
        PyHttpReceiverPtr receiver = m_factoryPyHttpReceiver->createObject( MENGINE_DOCUMENT_PYBIND );

        MENGINE_ASSERTION_MEMORY_PANIC( receiver );

        receiver->initialize( _cb, _args );

        HttpRequestPostProperties params;
        for( auto && [key, value] : _params )
        {
            String string_value;
            Helper::get( value, &string_value );

            params.emplace_back( HttpRequestPostProperty{key, string_value} );
        }

        HttpRequestId id = HTTP_SERVICE()
            ->postMessage( _url, {}, _timeout, false, params, receiver, MENGINE_DOCUMENT_PYBIND );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestId HttpServiceScriptEmbedding::headerData( const String & _url, const VectorString & _headers, const Data & _data, int32_t _timeout, const pybind::object & _cb, const pybind::args & _args )
    {
        PyHttpReceiverPtr receiver = m_factoryPyHttpReceiver->createObject( MENGINE_DOCUMENT_PYBIND );

        MENGINE_ASSERTION_MEMORY_PANIC( receiver );

        receiver->initialize( _cb, _args );

        HttpRequestId id = HTTP_SERVICE()
            ->headerData( _url, _headers, _timeout, false, _data, receiver, MENGINE_DOCUMENT_PYBIND );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestId HttpServiceScriptEmbedding::getMessage( const String & _url, int32_t _timeout, const pybind::object & _cb, const pybind::args & _args )
    {
        PyHttpReceiverPtr receiver = m_factoryPyHttpReceiver->createObject( MENGINE_DOCUMENT_PYBIND );

        MENGINE_ASSERTION_MEMORY_PANIC( receiver );

        receiver->initialize( _cb, _args );

        HttpRequestId id = HTTP_SERVICE()
            ->getMessage( _url, {}, _timeout, false, receiver, MENGINE_DOCUMENT_PYBIND );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    void HttpServiceScriptEmbedding::cancelRequest( HttpRequestId _id )
    {
        HTTP_SERVICE()
            ->cancelRequest( _id );
    }
    //////////////////////////////////////////////////////////////////////////
    bool HttpServiceScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::enum_<EHttpCode>( _kernel, "HttpCode" )
            // Informational
            .def( "HTTP_Continue", HTTP_Continue )
            .def( "HTTP_SwitchingProtocols", HTTP_SwitchingProtocols )
            .def( "HTTP_Processing", HTTP_Processing )
            .def( "HTTP_EarlyHints", HTTP_EarlyHints )
            
            // Successful
            .def( "HTTP_OK", HTTP_OK )
            .def( "HTTP_Created", HTTP_Created )
            .def( "HTTP_Accepted", HTTP_Accepted )
            .def( "HTTP_NonAuthoritativeInformation", HTTP_NonAuthoritativeInformation )
            .def( "HTTP_NoContent", HTTP_NoContent )
            .def( "HTTP_ResetContent", HTTP_ResetContent )
            .def( "HTTP_PartialContent", HTTP_PartialContent )
            .def( "HTTP_MultiStatus", HTTP_MultiStatus )
            .def( "HTTP_AlreadyReported", HTTP_AlreadyReported )
            .def( "HTTP_IMUsed", HTTP_IMUsed )

            // Redirection
            .def( "HTTP_MultipleChoices", HTTP_MultipleChoices )
            .def( "HTTP_MovedPermanently", HTTP_MovedPermanently )
            .def( "HTTP_Found", HTTP_Found )
            .def( "HTTP_SeeOther", HTTP_SeeOther )
            .def( "HTTP_NotModified", HTTP_NotModified )
            .def( "HTTP_UseProxy", HTTP_UseProxy )
            .def( "HTTP_TemporaryRedirect", HTTP_TemporaryRedirect )
            .def( "HTTP_PermanentRedirect", HTTP_PermanentRedirect )

            // Client Error
            .def( "HTTP_BadRequest", HTTP_BadRequest )
            .def( "HTTP_Unauthorized", HTTP_Unauthorized )
            .def( "HTTP_PaymentRequired", HTTP_PaymentRequired )
            .def( "HTTP_Forbidden", HTTP_Forbidden )
            .def( "HTTP_NotFound", HTTP_NotFound )
            .def( "HTTP_MethodNotAllowed", HTTP_MethodNotAllowed )
            .def( "HTTP_NotAcceptable", HTTP_NotAcceptable )
            .def( "HTTP_ProxyAuthenticationRequired", HTTP_ProxyAuthenticationRequired )
            .def( "HTTP_RequestTimeout", HTTP_RequestTimeout)
            .def( "HTTP_Conflict", HTTP_Conflict )
            .def( "HTTP_Gone", HTTP_Gone )
            .def( "HTTP_LengthRequired", HTTP_LengthRequired )
            .def( "HTTP_PreconditionFailed", HTTP_PreconditionFailed )
            .def( "HTTP_ContentTooLarge", HTTP_ContentTooLarge )
            .def( "HTTP_PayloadTooLarge", HTTP_PayloadTooLarge )
            .def( "HTTP_URITooLong", HTTP_URITooLong )
            .def( "HTTP_UnsupportedMediaType", HTTP_UnsupportedMediaType )
            .def( "HTTP_RangeNotSatisfiable" , HTTP_RangeNotSatisfiable )
            .def( "HTTP_ExpectationFailed" , HTTP_ExpectationFailed )
            .def( "HTTP_ImATeapot" , HTTP_ImATeapot )
            .def( "HTTP_MisdirectedRequest" , HTTP_MisdirectedRequest )
            .def( "HTTP_UnprocessableContent" , HTTP_UnprocessableContent )
            .def( "HTTP_UnprocessableEntity" , HTTP_UnprocessableEntity )
            .def( "HTTP_Locked" , HTTP_Locked )
            .def( "HTTP_FailedDependency" , HTTP_FailedDependency )
            .def( "HTTP_TooEarly" , HTTP_TooEarly )
            .def( "HTTP_UpgradeRequired" , HTTP_UpgradeRequired )
            .def( "HTTP_PreconditionRequired"   , HTTP_PreconditionRequired )
            .def( "HTTP_TooManyRequests" , HTTP_TooManyRequests )
            .def( "HTTP_RequestHeaderFieldsTooLarge" , HTTP_RequestHeaderFieldsTooLarge )
            .def( "HTTP_UnavailableForLegalReasons" , HTTP_UnavailableForLegalReasons )

            // Server Error
            .def( "HTTP_InternalServerError" , HTTP_InternalServerError )
            .def( "HTTP_NotImplemented" , HTTP_NotImplemented )
            .def( "HTTP_BadGateway" , HTTP_BadGateway )
            .def( "HTTP_ServiceUnavailable" , HTTP_ServiceUnavailable )
            .def( "HTTP_GatewayTimeout" , HTTP_GatewayTimeout )
            .def( "HTTP_HTTPVersionNotSupported" , HTTP_HTTPVersionNotSupported )
            .def( "HTTP_VariantAlsoNegotiates"  , HTTP_VariantAlsoNegotiates )
            .def( "HTTP_InsufficientStorage" , HTTP_InsufficientStorage )
            .def( "HTTP_LoopDetected" , HTTP_LoopDetected )
            .def( "HTTP_NotExtended" , HTTP_NotExtended )
            .def( "HTTP_NetworkAuthenticationRequired" , HTTP_NetworkAuthenticationRequired )
            ;

        pybind::def_functor_args( _kernel, "getMessage", this, &HttpServiceScriptEmbedding::getMessage );
        pybind::def_functor_args( _kernel, "postMessage", this, &HttpServiceScriptEmbedding::postMessage );
        pybind::def_functor_args( _kernel, "headerData", this, &HttpServiceScriptEmbedding::headerData );
        pybind::def_functor_args( _kernel, "downloadAsset", this, &HttpServiceScriptEmbedding::downloadAsset );
        pybind::def_functor( _kernel, "cancelRequest", this, &HttpServiceScriptEmbedding::cancelRequest );

        m_factoryPyHttpReceiver = Helper::makeFactoryPool<PyHttpReceiver, 32>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void HttpServiceScriptEmbedding::eject( pybind::kernel_interface * _kernel )
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

