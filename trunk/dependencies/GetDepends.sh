getdepend()
{
	if [ test -s $2 ];
	then echo "already download" $2
	else ftp $1
	fi

	rm -rf $4
	7z x -y $2
	mv $3 $4
}

getdepend_tgz()
{
	if [ test -s $2 ];
	then echo "already download" $2
	else ftp $1
	fi

	rm -rf $4
	tar -zxf $2
	mv $3 $4
}

svndepend()
{
    svn checkout -q $1 $2
}

getdepend http://zlib.net/zlib128.zip zlib128.zip zlib-1.2.8 zlib
getdepend http://download.sourceforge.net/libpng/lpng1616.zip lpng1616.zip lpng1616 libpng
getdepend http://www.ijg.org/files/jpegsr9a.zip jpegsr9a.zip jpeg-9a libjpeg
getdepend http://zlib.net/zlib128.zip zlib128.zip zlib-1.2.8 zlib
getdepend http://downloads.xiph.org/releases/ogg/libogg-1.3.2.zip libogg-1.3.2.zip libogg-1.3.2 libogg
getdepend http://downloads.xiph.org/releases/vorbis/libvorbis-1.3.4.zip libvorbis-1.3.4.zip libvorbis-1.3.4 libvorbis
getdepend http://downloads.xiph.org/releases/theora/libtheora-1.1.1.zip libtheora-1.1.1.zip libtheora-1.1.1 libtheora
getdepend2 http://kcat.strangesoft.net/openal-releases/openal-soft-1.16.0.tar.bz2 openal-soft-1.16.0.tar.bz2 openal-soft-1.16.0.tar openal-soft-1.16.0 openal-soft

getdepend_tgz https://www.python.org/ftp/python/2.7.10/Python-2.7.10.tgz Python-2.7.10.tgz Python-2.7.10.tar Python-2.7.10 Python
getdepend_tgz https://www.python.org/ftp/python/3.4.3/Python-3.4.3.tgz Python-3.4.3.tgz Python-3.4.3.tar Python-3.4.3 Python3

svndepend http://pugixml.googlecode.com/svn/trunk pugixml
svndepend https://svn.code.sf.net/p/pybind/code/trunk pybind
svndepend https://svn.code.sf.net/p/metabuf/code/trunk metabuf
svndepend https://svn.code.sf.net/p/stdex2/code/trunk stdex
svndepend http://svn.code.sf.net/p/utfcpp/code/v2_0 utf8
svndepend http://lz4.googlecode.com/svn/trunk lz4
svndepend https://svn.code.sf.net/p/fastpathfinder/code fastpathfinder
