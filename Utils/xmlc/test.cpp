#	include <stdio.h>

#	include <string>

#	include "XmlCompile.h"

int main()
{
	XmlCompile xmlc;

	xmlc.load( "format.xml" );

	xmlc.compile( "test.xml", "test.xmlc" );
}
