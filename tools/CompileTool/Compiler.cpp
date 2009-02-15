#	include "Compiler.h"
#	include "FreeImage.h"
#	include "pyCallback.h"
#	include "Makedirs.h"

#	include "CopyFileCallback.h"
#	include "CopyFolderCallback.h"
#	include "ResourceCallback.h"


#	include <algorithm>
#	include <direct.h>

//////////////////////////////////////////////////////////////////////////
Compiler::Compiler()
	: m_outputFolder("")
	, m_jpegQuality(100)
	, m_useMne(false)
{
	FreeImage_Initialise( false );
}
//////////////////////////////////////////////////////////////////////////
Compiler::~Compiler()
{
	for( TMapCallback::iterator it = m_callbacks.begin(); it != m_callbacks.end(); it++ )
	{
		delete it->second;
	}

	FreeImage_DeInitialise();
}
//////////////////////////////////////////////////////////////////////////
void Compiler::setJPEGQuality( int _quality )
{
	m_jpegQuality = _quality;
}
//////////////////////////////////////////////////////////////////////////
int Compiler::getJPEGQuality () const
{
	return m_jpegQuality;
}
//////////////////////////////////////////////////////////////////////////
void Compiler::setPNG_MNE( bool _value )
{
	m_useMne = _value;
}
//////////////////////////////////////////////////////////////////////////
bool Compiler::getPNG_MNE() const
{
	return m_useMne;
}
//////////////////////////////////////////////////////////////////////////
bool Compiler::registerCppCallback( const std::string & _name, Callback * callback )
{
	m_callbacks.insert( std::make_pair( _name, callback ) );
	return true;
}
//////////////////////////////////////////////////////////////////////////
bool Compiler::registerPyCallback( const std::string & _name, 
										const std::string & _module )
{
	PyObject * module = pybind::module_import( _module.c_str() );

	if( module == NULL )
	{
		return false;
	}

	PyObject * callback = pybind::get_attr( module, "callback" );

	if( callback == NULL )
	{
		return false;
	}

	Callback * pycallback = new pyCallback( callback );

	m_callbacks.insert( std::make_pair( _name, pycallback ) );

	return true;
}
//////////////////////////////////////////////////////////////////////////
Callback * Compiler::getCallback( const std::string & _callback )
{
	TMapCallback::iterator it = m_callbacks.find( _callback );

	if( it != m_callbacks.end() )
	{
		return it->second;
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////////
void Compiler::setOutputFolder( const std::string & _folder )
{
	m_outputFolder = _folder;
}
//////////////////////////////////////////////////////////////////////////
const Menge::String & Compiler::getOutputFolder() const
{
	return m_outputFolder;
}
//////////////////////////////////////////////////////////////////////////
void Compiler::compile( Menge::Application * _app, Menge::Game * _game )
{
	// 1. create output folder:

	mkdir( m_outputFolder.c_str() );

	// 2. collect all info about game:

	const std::string & gameIni = _app->getPathGameFile();
	const TStringVector & resourcePaths = _game->getResourcePaths();	
	const TStringVector & scriptPaths = _game->getScriptsPaths();	
	const TStringVector & entitiesPaths = _game->getEntitiesPaths();	
	const TStringVector & arrowPaths = _game->getArrowPaths();	
	const TStringVector & scenePaths = _game->getScenesPaths();	
	const TStringVector & textsPaths = _game->getTextsPaths();
	const TStringVector & resourceFilePaths = _game->getResourceFilePaths();	

	CopyFolderCallback * rcf = new CopyFolderCallback();

	rcf->pushExtFilter(".py");
	rcf->pushExtFilter(".pyc");
	rcf->pushExtFilter(".log");
	rcf->pushFileFilter("thumbs.db");
	rcf->pushFolderFilter(".svn");
	rcf->pushFolderFilter("thumbnails");

	registerCppCallback( "copyFolder", rcf );
	registerCppCallback( "convertResources", new ResourceCallback() );
	registerCppCallback( "copyFile", new CopyFileCallback() );

	Callback * copyFolderCallback = getCallback("copyFolder");
	Callback * copyFileCallback = getCallback("copyFile");
	Callback * convertResourcesCallback = getCallback("convertResources");

	copy( gameIni, m_outputFolder, copyFileCallback );

	copy( resourcePaths, m_outputFolder, copyFileCallback );
	copy( textsPaths, m_outputFolder, copyFileCallback );

	copy( scriptPaths, m_outputFolder, copyFolderCallback );
	copy( entitiesPaths, m_outputFolder, copyFolderCallback );
	copy( scenePaths, m_outputFolder, copyFolderCallback );
	copy( arrowPaths, m_outputFolder, copyFolderCallback );
	copy( "Bin", m_outputFolder, copyFolderCallback );

	copy( resourceFilePaths, m_outputFolder, convertResourcesCallback );
}
//////////////////////////////////////////////////////////////////////////
bool Compiler::copy( const TStringVector & _paths, const std::string & _dest, Callback * _callback)
{
	std::for_each( _paths.begin(), _paths.end(), 
		std::bind2nd( Makedirs( _callback, this ), _dest ) );

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool Compiler::copy( const std::string & _path, const std::string & _dest, Callback * _callback)
{
	(Makedirs(_callback, this))(_path, _dest);
	return true;
}
//////////////////////////////////////////////////////////////////////////