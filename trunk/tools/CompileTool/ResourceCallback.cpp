#	include "ResourceCallback.h"
#	include "ConvertResourceVisitor.h"
//////////////////////////////////////////////////////////////////////////
ResourceCallback::ResourceCallback()
{}
//////////////////////////////////////////////////////////////////////////
bool ResourceCallback::execute( const Menge::String & _src, const Menge::String & _dst, Compiler * _compiler )
{
	printf("PROCESS %s\n", _src.c_str());

	Menge::ConvertResourceVisitor convertResourceVisitor( _src, _compiler );
		
	Menge::Holder<Menge::ResourceManager>::hostage()->visitResources(
		&convertResourceVisitor, _src );

	return true;
}