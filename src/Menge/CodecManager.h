#	include "Codec.h"

namespace Menge
{
	class CodecManager
	{
	public:
		CodecManager();
		~CodecManager();

	public:
		void registerCodec( Codec *_codec );
		void unregisterCodec( Codec * _codec );

		void initialize();
		void cleanup();
		Codec * getCodec( const String& _extension );

	private:
		typedef std::map< String, Codec* > TCodecMap; 
		TCodecMap ms_mapCodecs;
	};
}