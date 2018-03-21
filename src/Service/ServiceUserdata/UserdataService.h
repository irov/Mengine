#pragma once

#include "Interface/UserdataInterface.h"
#include "Interface/ArchiveInterface.h"

#include "Core/ServiceBase.h"

#include "stdex/stl_map.h"

namespace Mengine
{
    class UserdataService
		: public ServiceBase<UserdataServiceInterface>
    {
    public:
		UserdataService();
		~UserdataService() override;

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		bool addUserdata( const ConstString & _name, const ConstString & _category, const FilePath & _path ) override;
		bool removeUserdata( const ConstString & _name ) override;
		bool hasUserdata( const ConstString & _name ) const override;

	public:
		MemoryInterfacePtr loadUserdata( const ConstString & _name ) const override;
		bool writeUserdata( const ConstString & _name, const void * _data, size_t _size ) const override;

	protected:
		ArchivatorInterfacePtr m_archivator;

		struct UserdataDesc
		{
			ConstString category;
			FilePath path;
		};

		typedef stdex::map<ConstString, UserdataDesc> TMapDatas;
		TMapDatas m_datas;
    };
}