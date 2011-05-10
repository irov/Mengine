#	pragma once

#	include "Config/Typedef.h"

#	include "Core/Holder.h"
#	include "Core/String.h"

#	include "FactoryManager.h"

#	include <map>

namespace Menge
{
	typedef std::map<String, TVectorString> TMapParam;
	typedef std::map<ConstString, TMapParam> TMapParams;

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
