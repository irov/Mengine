#	pragma once

#	include "Callback.h"

#	include "Menge/Application.h"

class CopyFolderCallback
	: public Callback
{
public:
	CopyFolderCallback();
	~CopyFolderCallback();

public:
	void pushExtFilter( const Menge::String & _extension );
	void pushFileFilter( const Menge::String & _file );
	void pushFolderFilter( const Menge::String & _folder );

	bool isExtFilter( const Menge::String & _extension );
	bool isFileFilter( const Menge::String & _file );
	bool isFolderFilter( const Menge::String & _folder );

	bool execute( const std::string & _src, const std::string & _dst, Compiler * );

private:

	typedef std::vector<std::string> TFilterVec;

	TFilterVec m_extFilter;
	TFilterVec m_fileFilter;
	TFilterVec m_folderFilter;

	Menge::String _getExtension(const Menge::String & _filename);

	bool _recursiveCopyFolder( Menge::StringW csPath, Menge::StringW csNewPath );
};