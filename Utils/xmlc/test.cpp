#	include <stdio.h>

#	include <string>

#	include "format.h"

int main()
{
	XmlCompile xmlc;

	xmlc.load( "format.xml" );

	xmlc.compile( "test.xml", "test.xmlc" );
}
