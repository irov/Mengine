#	pragma once

#	include "Config/Typedef.h"

#	include "Core/Holder.h"
#	include "Core/String.h"

#	include "FactoryManager.h"

#	include <map>

namespace Menge
{
	struct Params
	{
		typedef std::vector<TVectorString> TVectorParam;
		typedef std::map<String, TVectorParam> TMapParam;

		TMapParam mp;
	};

	typedef std::map<ConstString, Params> TMapParams;

	class ParamManager
		: public Holder<ParamManager>
		, public FactoryManager
	{
	public:
		ParamManager();
		~ParamManager();

	public:
		const TMapParams & getParams() const;

	public:
		bool loadParam( const ConstString& _category, const ConstString& _group, const String& _file );

	protected:
		TMapParams m_params;
	};
}
