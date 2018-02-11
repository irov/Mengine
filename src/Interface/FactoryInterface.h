#	pragma once

#	include "Config/Typedef.h"

#	include "Interface/ServiceInterface.h"

namespace Menge
{	
	//////////////////////////////////////////////////////////////////////////
	class Factory;
	//////////////////////////////////////////////////////////////////////////
	class VisitorFactoryService
	{
	public:
		virtual void visit(const Factory * _factory) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	class FactoryServiceInterface
		: public ServiceInterface
	{
		SERVICE_DECLARE("FactoryService")

	public:
		virtual void registerFactory( const Factory * _factory) = 0;
		virtual void unregisterFactory( const Factory * _factory) = 0;

	public:
		virtual void visitFactories(VisitorFactoryService * _visitor) = 0;
	};
}

#   define FACTORY_SERVICE()\
	((FactoryServiceInterface *)SERVICE_GET(Menge::FactoryServiceInterface))
