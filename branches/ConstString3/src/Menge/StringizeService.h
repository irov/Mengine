#	pragma once

#	include "Interface\StringizeInterface.h"

namespace Menge
{	
	//////////////////////////////////////////////////////////////////////////
	class StringizeService
		: public StringizeServiceInterface
	{
	public:
		const ConstString & stringize( const String & _str ) override;

	protected:
		typedef std::vector<ConstString> TStringize;
		TStringize m_strigized;
	};
}
