wget
[wget]
http://users.ugent.be/~bpuype/cgi-bin/fetch.pl?dl=wget/wget.exe

run getDepends.bat

-------------------------------------------------------------------------------

DXSDK8
[DirectX/DX8]
ftp://ftp.qut.edu.au/pub/mirrors/microsoft/directx/directx8/dx81b_sdk.exe
- only includes [DirectX/DX8/include]
- only libs [DirectX/DX8/lib/dxguid.lib]
- delete DirectX/DX8/include/basetsd.h

astralax
[magic particles]
http://www.astralax.ru/download/programm/particles_dev.exe
-copy c:\Program Files\Magic Particles (Dev)\SDK\dll

--------------------------------------------------------------------------------
COMMENTS:
-- in Python/PC/pyconfig.h rewrite

#define PY_UNICODE_TYPE wchar_t

-- MinGWPatch - dx8 patch for mingw

--------------------------------------------------------------------------------

+xml2bin

--------------------------------------------------------------------------------
tinyxml
[tinyxml]
http://downloads.sourceforge.net/project/tinyxml/tinyxml/2.6.1/tinyxml_2_6_1.zip?r=http%3A%2F%2Fsourceforge.net%2Fprojects%2Ftinyxml%2F&ts=1285943527&use_mirror=sunet
