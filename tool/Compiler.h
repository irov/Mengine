#	pragma once

#	include "Menge/Game.h"

#	include "Callback.h"

typedef std::map<std::string, Callback *> TMapCallback;

class Compiler
{
public:
	Compiler();
	~Compiler();

public:

	void compile( Menge::Game * _game );

	bool copy( const TStringVector & _paths, const std::string & _dest, Callback * _callback);
	bool copy( const std::string & _path, const std::string & _dest, Callback * _callback);

	void setOutputFolder( const std::string & _folder );
	const std::string & getOutputFolder() const;

	bool registerCppCallback( const std::string & _name, Callback * callback );
	bool registerPyCallback( const std::string & _name, const std::string & _module );

	Callback * getCallback( const std::string & _callback );

private:
	std::string m_outputFolder;

	TMapCallback m_callbacks;
};