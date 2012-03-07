#	include "StringizeService.h"

#	include <algorithm>

namespace Menge
{	
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class FStringizeFind
		{
		public:
			FStringizeFind( const String & _str )
				: m_str(_str)
			{
			}

		public:
			bool operator() ( const ConstString & _cstr ) const
			{
				const String & str = _cstr.to_str();

				if( m_str != str )
				{
					return false;
				}

				return true;
			}

		protected:
			const String & m_str;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & StringizeService::stringize( const String & _str )
	{
		TStringize::const_iterator it_found = std::find_if(m_strigized.begin(), m_strigized.end(), FStringizeFind(_str));

		if( it_found == m_strigized.end() )
		{
			ConstString cstr(_str);
			it_found = m_strigized.insert( m_strigized.end(), cstr );
		}
								
		return *it_found;
	}
	
}
