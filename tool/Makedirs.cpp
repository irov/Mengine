#	include "Makedirs.h"
#	include "Callback.h"

#	include <vector>

#	include "Menge/Utils.h"

#	include <direct.h>

//////////////////////////////////////////////////////////////////////////
std::string joinPath( const std::string& _base, const std::string& _name )
{
	if ( _base.empty() || _name[1] == ':' || _name[0] == '/' || _name[0] == '\\' )
	{
		return _name;
	}
	else
	{
		return _base + '/' + _name;
	}
}
//////////////////////////////////////////////////////////////////////////
Makedirs::Makedirs( Callback * _callback, Compiler * _compiler )
	: m_callback( _callback )
	, m_compiler( _compiler )
{}
//////////////////////////////////////////////////////////////////////////
bool Makedirs::operator()( const std::string & _src, const std::string & _dst ) const
{
	std::vector<std::string> files = Menge::Utils::split( _src, "/\\" );

	if( files.empty() )
	{
		return false;
	}

	std::string srcPath = "";
	std::string dstPath = _dst;

	for( TStringVector::iterator it = files.begin();
		it != files.end() - 1;
		it++ )
	{
		const Menge::String & file = *it;

		srcPath = joinPath( srcPath, file );
		dstPath = joinPath( dstPath, file );

		mkdir( dstPath.c_str() );
	}

	std::string & targetFile = files[ files.size() - 1 ];

	srcPath = joinPath( srcPath, targetFile );
	dstPath = joinPath( dstPath, targetFile );

	if( m_callback != NULL )
	{
		return m_callback->execute( srcPath, dstPath, m_compiler );
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////