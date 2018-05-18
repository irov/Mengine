#pragma once

#include "Interface/HttpSystemInterface.h"

#include "Core/ServiceBase.h"

#include "Factory/Factory.h"

#include "Kernel/ThreadTask.h"

namespace Mengine
{
	class MockupHttpSystem
		: public ServiceBase<HttpSystemInterface>
		, public HttpReceiverInterface
	{
	public:
		MockupHttpSystem();

	protected:
		bool _initializeService() override;
		void _finalizeService() override;

	protected:
		HttpRequestID getMessage( const String & _url, const HttpReceiverInterfacePtr & _receiver ) override;
		HttpRequestID postMessage( const String & _url, const TMapParams & _params, const HttpReceiverInterfacePtr & _receiver ) override;
		HttpRequestID downloadAsset( const String & _url, const String & _login, const String & _password, const ConstString & _category, const FilePath & _path, const HttpReceiverInterfacePtr & _receiver ) override;
        HttpRequestID headerData( const String & _url, const TVectorString & _headers, const String & _data, const HttpReceiverInterfacePtr & _receiver ) override;
        
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
            HttpReceiverInterfacePtr receiver;
		};

		typedef stdex::vector<HttpReceiverDesc> TVectorHttpReceiverDesc;
		TVectorHttpReceiverDesc m_receiverDescs;

		FactoryPtr m_factoryTaskDummy;

	};
}
