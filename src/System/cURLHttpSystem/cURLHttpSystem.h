#	pragma once

#	include "Interface/HttpSystemInterface.h"

#	include "Core/ServiceBase.h"

#	include "Factory/Factory.h"

#	include "Kernel/ThreadTask.h"

namespace Mengine
{
	class cURLHttpSystem
		: public ServiceBase<HttpSystemInterface>
		, public HttpReceiver
	{
	public:
		cURLHttpSystem();

	protected:
		bool _initialize() override;
		void _finalize() override;

	protected:
		HttpRequestID getMessage( const String & _url, HttpReceiver * _receiver ) override;
		HttpRequestID postMessage( const String & _url, const TMapParams & _params, HttpReceiver * _receiver ) override;
        HttpRequestID headerData( const String & _url, const TVectorString & _headers, const String & _data, HttpReceiver * _receiver ) override;

    protected:
		HttpRequestID downloadAsset( const String & _url, const String & _login, const String & _password, const ConstString & _category, const FilePath & _path, HttpReceiver * _receiver ) override;
        
	protected:
		bool cancelRequest( HttpRequestID _id ) override;

	private:
		void onHttpRequestComplete( HttpRequestID _id, uint32_t _status, const String & _response, uint32_t _code, bool _successful ) override;

	protected:
		uint32_t m_enumeratorReceivers;

		struct HttpReceiverDesc
		{
			uint32_t id;
			ThreadTaskPtr task;
			HttpReceiver * receiver;
		};

		typedef stdex::vector<HttpReceiverDesc> TVectorHttpReceiverDesc;
		TVectorHttpReceiverDesc m_receiverDescs;

		FactoryPtr m_factoryTaskGetMessage;
		FactoryPtr m_factoryTaskPostMessage;
        FactoryPtr m_factoryTaskHeaderData;
		FactoryPtr m_factoryTaskDownloadAsset;
	};
}