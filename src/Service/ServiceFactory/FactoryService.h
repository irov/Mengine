#	pragma once

#	include "Interface/FactoryInterface.h"

#   include "Core/ServiceBase.h"

#	include "stdex/stl_vector.h"

namespace Menge
{
	class FactoryService
		: public ServiceBase<FactoryServiceInterface>
	{
	public:
		FactoryService();
		~FactoryService();

	public:
		void registerFactory(const Factory * _factory) override;
		void unregisterFactory( const Factory * _factory) override;

	public:
		void visitFactories(VisitorFactoryService * _visitor) override;

	protected:
		typedef stdex::vector<const Factory *> TVectorFactories;
		TVectorFactories m_factories;
	};
}