#	pragma once

#	include "Config/Typedef.h"

#	include "Core/Holder.h"
#	include "Core/String.h"

#	include "FactoryManager.h"

#	include <map>

namespace Menge
{
	struct Param
	{
		String file;
		ConstString category;
	};
	
	typedef std::vector<TVectorString> TVectorParams;
	typedef std::map<ConstString, Param> TMapParams;

	class ParamManager
		: public Holder<ParamManager>
		, public FactoryManager
	{
	public:
		ParamManager();
		~ParamManager();

	public:
		bool registerParam( const ConstString& _category, const ConstString& _group, const String& _file );
		bool getParam( const ConstString& _group, TVectorParams & _params );

	protected:
		TMapParams m_params;
	};
}
