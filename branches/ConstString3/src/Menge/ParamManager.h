//#	pragma once
//
//#	include "Config/Typedef.h"
//
//#	include "Core/Holder.h"
//#	include "Core/String.h"
//
//#	include "FactoryManager.h"
//#	include "ResourceManager.h"
//
//#	include <map>
//
//namespace Menge
//{
//	typedef std::vector<TVectorString> TVectorParams;
//	typedef std::map<ConstString, ResourceDesc> TMapParams;
//
//	class ParamManager
//		: public Holder<ParamManager>
//		, public FactoryManager
//	{
//	public:
//		ParamManager();
//		~ParamManager();
//
//	public:
//		bool registerParam( const ConstString& _name, const ResourceDesc & _desc );
//		bool getParam( const ConstString& _name, TVectorParams & _params );
//
//	protected:
//		TMapParams m_params;
//	};
//}
