#	include <stdio.h>

#	include <string>

#	include "XmlCompile.h"

int main()
{
	XmlCompile xmlc;

	strcmp( "abcdddd", "blablo" );

	xmlc.load( "format.xml" );

	xmlc.compile( "test.xml", "test.xmlc" );

	//xmlc.parse( "test.xmlc" );
}
