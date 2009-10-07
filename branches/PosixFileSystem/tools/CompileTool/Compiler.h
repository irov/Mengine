#	pragma once

#	include "Menge/Game.h"
#	include "Menge/Application.h"

#	include "Callback.h"

typedef std::map<std::string, Callback *> TMapCallback;

class Compiler
{
public:
	Compiler();
	~Compiler();

public:

	void setJPEGQuality( int _quality );
	int getJPEGQuality () const;

	void setPNG_MNE( bool _value );
	bool getPNG_MNE() const;

	void compile( Menge::Application * _app, Menge::Game * _game );

	bool copy( const TStringVector & _paths, const std::string & _dest, Callback * _callback);
	bool copy( const std::string & _path, const std::string & _dest, Callback * _callback);

	void setOutputFolder( const std::string & _folder );
	const std::string & getOutputFolder() const;

	bool registerCppCallback( const std::string & _name, Callback * callback );
	bool registerPyCallback( const std::string & _name, const std::string & _module );

	Callback * getCallback( const std::string & _callback );

private:
	int m_jpegQuality;
	bool m_useMne;

	std::string m_outputFolder;

	TMapCallback m_callbacks;
};