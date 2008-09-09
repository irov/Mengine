#	pragma once

#	include "Config/Typedef.h"

#	include <map>

namespace Menge 
{
	class CodecInterface;

	class CodecManager
	{
	public:
		static void registerCodec( const StringA& _type, CodecInterface* _codec );
		static void unregisterCodec( const StringA& _type );

		static void initialize();
		static void cleanup();
	
		static CodecInterface* getCodec( const StringA& _type );

	protected:
		typedef std::map< StringA, CodecInterface* > TCodecMap; 
		static TCodecMap ms_mapCodecs;
	};
} // namespace Menge
