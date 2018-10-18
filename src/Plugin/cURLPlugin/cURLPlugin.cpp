#include "cURLPlugin.h"

#include "Interface/FileSystemInterface.h"
#include "Interface/ScriptSystemInterface.h"
#include "Interface/StringizeInterface.h"

#include "Kernel/FactoryPool.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( cURLService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( cURL, Mengine::cURLPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    cURLPlugin::cURLPlugin()
    {
    }
	//////////////////////////////////////////////////////////////////////////
    cURLPlugin::~cURLPlugin()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class PyCURLReceiver
            : public Factorable
            , public cURLReceiverInterface
        {
        public:
            PyCURLReceiver()
            {
            }

            ~PyCURLReceiver() override
            {
            }

        public:
            void initialize( const pybind::object & _cb, const pybind::args & _args )
            {
                m_cb = _cb;
                m_args = _args;
            }

        protected:
            void onHttpRequestComplete( HttpRequestID _id, uint32_t _status, const String & _error, const String & _response, uint32_t _code, bool _successful ) override
            {
                m_cb.call_args( _id, _status, _error, _response, _code, _successful, m_args );
            }

        protected:
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PyCURLReceiver> PyCURLReceiverPtr;
        //////////////////////////////////////////////////////////////////////////        
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID cURLPlugin::downloadAsset( const String & _url, const String & _login, const String & _password, const ConstString & _category, const FilePath & _filepath, const pybind::object & _cb, const pybind::args & _args )
    {
        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getFileGroup( _category );

        if( fileGroup == nullptr )
        {
            return 0;
        }

        Detail::PyCURLReceiverPtr receiver = m_factoryPyHttpReceiver->createObject();

        receiver->initialize( _cb, _args );

        uint32_t id = CURL_SERVICE()
            ->downloadAsset( _url, _login, _password, fileGroup, _filepath, receiver );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID cURLPlugin::postMessage( const String & _url, const MapParams & _params, const pybind::object & _cb, const pybind::args & _args )
    {
        Detail::PyCURLReceiverPtr receiver = m_factoryPyHttpReceiver->createObject();

        receiver->initialize( _cb, _args );

        HttpRequestID id = CURL_SERVICE()
            ->postMessage( _url, _params, receiver );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID cURLPlugin::headerData( const String & _url, const VectorString & _headers, const String & _data, const pybind::object & _cb, const pybind::args & _args )
    {
        Detail::PyCURLReceiverPtr receiver = m_factoryPyHttpReceiver->createObject();

        receiver->initialize( _cb, _args );

        HttpRequestID id = CURL_SERVICE()
            ->headerData( _url, _headers, _data, receiver );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID cURLPlugin::getMessage( const String & _url, const pybind::object & _cb, const pybind::args & _args )
    {
        Detail::PyCURLReceiverPtr receiver = m_factoryPyHttpReceiver->createObject();

        receiver->initialize( _cb, _args );

        HttpRequestID id = CURL_SERVICE()
            ->getMessage( _url, receiver );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLPlugin::cancelRequest( HttpRequestID _id )
    {
        CURL_SERVICE()
            ->cancelRequest( _id );
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLPlugin::_initialize()
    {
        SERVICE_CREATE( cURLService );

        m_factoryPyHttpReceiver = new FactoryPool<Detail::PyCURLReceiver, 32>();

        pybind::kernel_interface * kernel = SCRIPT_SERVICE()
            ->getKernel();

        pybind::set_kernel( kernel );

        pybind::def_functor_args( kernel, "getMessage", this, &cURLPlugin::getMessage );
        pybind::def_functor_args( kernel, "postMessage", this, &cURLPlugin::postMessage );
        pybind::def_functor_args( kernel, "headerData", this, &cURLPlugin::headerData );
        pybind::def_functor_args( kernel, "downloadAsset", this, &cURLPlugin::downloadAsset );
        pybind::def_functor( kernel, "cancelRequest", this, &cURLPlugin::cancelRequest );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLPlugin::_finalize()
    {
        pybind::kernel_interface * kernel = SCRIPT_SERVICE()
            ->getKernel();

        kernel->remove_from_module( "getMessage", nullptr );
        kernel->remove_from_module( "postMessage", nullptr );
        kernel->remove_from_module( "headerData", nullptr );
        kernel->remove_from_module( "downloadAsset", nullptr );
        kernel->remove_from_module( "cancelRequest", nullptr );

        m_factoryPyHttpReceiver = nullptr;

        SERVICE_FINALIZE( Mengine::cURLServiceInterface );
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLPlugin::_destroy()
    {
        SERVICE_DESTROY( Mengine::cURLServiceInterface );
    }
}