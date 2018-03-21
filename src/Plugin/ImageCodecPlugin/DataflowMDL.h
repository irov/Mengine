#pragma once

#include "Interface/Model3DInterface.h"
#include "Interface/DataInterface.h"
#include "Interface/ArchiveInterface.h"

#include "Core/ServantBase.h"

#include "Model3DPack.h"

#include "Factory/Factory.h"

namespace Mengine
{
	class DataflowMDL
		: public ServantBase<DataflowInterface>
	{
	public:
		DataflowMDL();
		~DataflowMDL() override;

	public:
		bool initialize() override;
		void finalize() override;

	public:
		DataInterfacePtr create() override;
		bool load( const DataInterfacePtr & _data, const InputStreamInterfacePtr & _stream ) override;

	protected:
		ArchivatorInterfacePtr m_archivator;
        		
		FactoryPtr m_poolModel3DPack;
	};
}