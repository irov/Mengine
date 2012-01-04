#	pragma once
#	include "CURLHTTPUtils.h"
namespace Menge
{
	HTTPStringType CURLHTTPUtils::char2hex( char dec )
	{
		char dig1 = (dec&0xF0)>>4;
		char dig2 = (dec&0x0F);
		if ( 0<= dig1 && dig1<= 9) dig1+=48;    //0,48inascii
		if (10<= dig1 && dig1<=15) dig1+=97-10; //a,97inascii
		if ( 0<= dig2 && dig2<= 9) dig2+=48;
		if (10<= dig2 && dig2<=15) dig2+=97-10;

		HTTPStringType r;
		r.append( &dig1, 1);
		r.append( &dig2, 1);
		return r;
	}
	///////////////////////////////////////////////////////////////////////
	//based on javascript encodeURIComponent()
    HTTPStringType CURLHTTPUtils::urlencode(const HTTPStringType &c)
	{

		HTTPStringType escaped="";
		int max = c.length();
		for(int i=0; i<max; i++)
		{
			if ( (48 <= c[i] && c[i] <= 57) ||//0-9
				(65 <= c[i] && c[i] <= 90) ||//abc...xyz
				(97 <= c[i] && c[i] <= 122) || //ABC...XYZ
				(c[i]=='~' || c[i]=='!' || c[i]=='*' || c[i]=='(' || c[i]==')' || c[i]=='\'')
				)
			{
				escaped.append( &c[i], 1);
			}
			else
			{
				escaped.append("%");
				escaped.append( char2hex(c[i]) );//converts char 255 to string "ff"
			}
		}
		return escaped;
	}
	///////////////////////////////////////////////////////////////////////
}