#	pragma once

#	include "DllModule.h"

class FileSystemInterface;
class FileDataInterface;

class TiXmlElement;
class TiXmlDocument;

namespace RvEngine
{
	class FileEngine
		: public DllModuleInterface<FileSystemInterface>
	{
	public:
		FileEngine(const std::string &_dllModule);

	public:
		bool	loadPak(const std::string&	_filename, int _prior);
		bool	unloadPak(const std::string& _filename);
		bool	existFile(const std::string& _filename);
		bool	createFolder(const std::string&	_foldername);
		FileDataInterface*	openFile(const std::string&	_filename);
		void	closeFile(FileDataInterface* _fd);

	public:
		bool loadXml(TiXmlDocument &_xmlDocument, const std::string &_fileName);
	};
}

//////////////////////////////////////////////////////////////////////////
#	define XML_PARSE_FILE_EX( FILE_ENGINE_PTR, FILE_NAME )\
	for( bool irov_xml_parse_once = true ; irov_xml_parse_once == true; )\
	for( \
	TiXmlDocument irov_xml_document;\
	irov_xml_parse_once==true;\
	)\
	if( (irov_xml_parse_once = FILE_ENGINE_PTR->loadXml(irov_xml_document,FILE_NAME)) == true )\
	for(\
	TiXmlElement * irov_xml_current_tree = irov_xml_document.FirstChildElement();\
	irov_xml_current_tree != 0;\
	irov_xml_current_tree = irov_xml_current_tree->NextSiblingElement()\
	)\
	for( bool irov_xml_else_if = false; irov_xml_parse_once == true;irov_xml_parse_once = false)