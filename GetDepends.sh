basedir=${PWD##*/}
toolsdir=%basedir%\wget
dependenciesdir=%basedir%\dependencies

getdepend()
{
	if [ ! -f $2 ]; then
        curl -L $1 > $2
    fi

	rm -rf $4
	%toolsdir%/7za x -y $2
	mv $3 $4
}

getdepend2()
{
	if [ ! -f $2 ]; then
        curl -L $1 > $2
    fi

	rm -rf $5
	%toolsdir%/7za x -y $2
    %toolsdir%/7za x -y $3
	mv $4 $5
}

getdepend_tgz()
{
	if [ ! -f $2 ]; then
        curl -L $1 > $2
    fi

	rm -rf $4
	tar -zxf $2
	mv $3 $4
}

svndepend()
{
    svn checkout -q $1 $2
}

svndependr()
{
    svn checkout -q -r $3 $1 $2
}

mkdir %dependenciesdir%
cd %dependenciesdir%

getdepend http://downloads.sourceforge.net/project/boost/boost/1.63.0/boost_1_63_0.7z boost_1_63_0.7z boost_1_63_0 boost

getdepend http://download.sourceforge.net/libpng/lpng1628.zip lpng1628.zip lpng1628 libpng
getdepend http://www.ijg.org/files/jpegsr9b.zip jpegsr9b.zip jpeg-9b libjpeg
getdepend http://www.zlib.net/zlib-1.2.11.tar.gz zlib-1.2.11.tar zlib-1.2.11 zlib
getdepend http://downloads.xiph.org/releases/ogg/libogg-1.3.2.zip libogg-1.3.2.zip libogg-1.3.2 libogg
getdepend http://downloads.xiph.org/releases/vorbis/libvorbis-1.3.5.zip libvorbis-1.3.5.zip libvorbis-1.3.5 libvorbis
getdepend http://downloads.xiph.org/releases/theora/libtheora-1.1.1.zip libtheora-1.1.1.zip libtheora-1.1.1 libtheora

getdepend2 https://storage.googleapis.com/downloads.webmproject.org/releases/webp/libwebp-0.6.0.tar.gz libwebp-0.6.0.tar.gz libwebp-0.6.0.tar libwebp-0.6.0 libwebp
getdepend2 http://kcat.strangesoft.net/openal-releases/openal-soft-1.17.2.tar.bz2 openal-soft-1.17.2.tar.bz2 openal-soft-1.17.2.tar openal-soft-1.17.2 openal-soft

getdepend_tgz https://www.python.org/ftp/python/2.7.13/Python-2.7.13.tgz Python-2.7.13.tgz Python-2.7.13 Python
getdepend_tgz https://www.python.org/ftp/python/3.4.3/Python-3.4.3.tgz Python-3.4.3.tgz Python-3.4.3 Python3

svndepend https://github.com/zeux/pugixml.git/trunk pugixml
svndepend https://github.com/irov/pybind.git/trunk pybind
svndepend https://github.com/irov/metabuf.git/trunk metabuf
svndepend https://github.com/irov/stdex.git/trunk stdex
svndepend https://github.com/irov/movie.git/trunk libmovie
svndepend https://github.com/nemtrif/utfcpp.git/trunk utf8
svndepend https://github.com/lz4/lz4.git/trunk lz4
svndepend https://github.com/irov/fastpathfinder.git/trunk fastpathfinder
svndepend https://github.com/zzzzrrr/poly2tri/trunk poly2tri
svndepend svn://svn.code.sf.net/p/polyclipping/code/trunk/cpp polyclipping
svndepend https://github.com/erincatto/Box2D.git/trunk/Box2D Box2D
svndepend https://github.com/irov/GOAP.git/trunk GOAP

svndependr https://github.com/EsotericSoftware/spine-runtimes.git/trunk/spine-c spine-c 1587