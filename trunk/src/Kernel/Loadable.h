#	pragma once

namespace Metabuf
{
    class Metadata;
}

namespace Menge
{
	class Loadable
	{
	public:
		bool loader( const Metabuf::Metadata * _parser );
		
    public:
        virtual bool _loader( const Metabuf::Metadata * _parser ) = 0;

	};
}