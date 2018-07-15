#pragma once

#include "Interface/DataInterface.h"

#include "Kernel/ServantBase.h"
#include "Kernel/FactoryDefault.h"

#include "Kernel/Logger.h"

namespace Mengine
{     
	template<class T>
	class DataflowFactory
		: public ServantBase<DataflowFactoryInterface>
	{
    protected:
        bool initialize() override
        {
            m_factory = new FactoryDefault<T>();

            return true;
        }

	protected:
		DataflowInterfacePtr createDataflow() override
		{	
			DataflowInterfacePtr decoder = m_factory->createObject();

			if( decoder->initialize() == false )
			{
				LOGGER_ERROR("DataflowFactory::createDataflow invalid initalize"
					);

				return nullptr;
			}

			return decoder;
		}

	protected:
		void destroy() override
		{
			delete this;
		}

	protected:
		FactoryPtr m_factory;
	};
}
