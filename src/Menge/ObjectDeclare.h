#	pragma once

#	define OBJECT_DECLARE(C)\
	public:\
	static Node * genObject(const std::string &);\
	virtual void visit( Visitor * );\
	private: