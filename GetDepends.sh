basedir=${PWD}
dependenciesdir=$basedir/dependencies

getdepend()
{
	cd $dependenciesdir
	if [ ! -d $4 ]; then
		if [ ! -f $2 ]; then
			curl -L $1 > $2
		fi

		rm -rf $4
		unzip $2
		mv $3 $4
	fi
}

getdependt()
{
	cd $dependenciesdir
	if [ ! -d $4 ]; then
		if [ ! -f $2 ]; then
			curl -L $1 > $2
		fi

		rm -rf $4
		tar -xf $2
		mv $3 $4
	fi
}

getdepend7()
{
	cd $dependenciesdir
	if [ ! -d $4 ]; then
		if [ ! -f $2 ]; then
			curl -L $1 > $2
		fi

		rm -rf $4
		./../wget/7za x -y $2
		mv $3 $4
	fi
}

svndepend()
{
    cd $dependenciesdir
	if [ ! -d $2 ]; then
		svn checkout -q $1 $2
	fi
}

svndependr()
{
    cd $dependenciesdir
	if [ ! -d $2 ]; then
		svn checkout -q -r $3 $1 $2
	fi
}

gitdepend()
{
	cd $dependenciesdir
	if [ ! -d $2 ]; then
		git clone --recurse-submodules $1 $2
	fi
}

gitdependr()
{
	cd $dependenciesdir
	if [ ! -d $2 ]; then
		git clone --recurse-submodules -b $3 $1 $2
	fi
}

if [ ! -d $dependenciesdir ]; then
    mkdir $dependenciesdir
fi

getdepend http://www.ijg.org/files/jpegsr9c.zip jpegsr9c.zip jpeg-9c libjpeg
getdepend http://www.astralax.ru/download/programm/dev/magic_win.zip magic_win.zip win astralax
getdepend https://github.com/leenjewel/openssl_for_ios_and_android/releases/download/android_openssl-1.1.0f_curl-7.54.1/only_android_openssl-1.1.0f_curl-7.54.1.zip only_android_openssl-1.1.0f_curl-7.54.1.zip libs curl-android

gitdepend https://github.com/boostorg/boost.git boost
gitdepend https://github.com/madler/zlib.git zlib
gitdepend https://github.com/xiph/ogg.git libogg
gitdepend https://github.com/xiph/vorbis.git libvorbis
gitdepend https://github.com/aseprite/freetype2.git freetype
gitdepend https://github.com/webmproject/libwebp.git libwebp
gitdepend https://github.com/zeux/pugixml.git pugixml
gitdepend https://github.com/irov/pybind.git pybind
gitdepend https://github.com/irov/metabuf.git metabuf
gitdepend https://github.com/irov/stdex.git stdex
gitdepend https://github.com/irov/movie.git libmovie
gitdepend https://github.com/nemtrif/utfcpp.git utf8
gitdepend https://github.com/lz4/lz4.git lz4
gitdepend https://github.com/irov/fastpathfinder.git fastpathfinder
gitdepend https://github.com/zzzzrrr/poly2tri poly2tri
gitdepend https://github.com/erincatto/Box2D.git Box2D
gitdepend https://github.com/irov/GOAP.git GOAP
gitdepend https://github.com/curl/curl.git libcurl
gitdepend https://github.com/irov/tinyini.git tinyini
gitdepend https://github.com/spurious/SDL-mirror.git SDL2
gitdepend https://github.com/frankinshtein/font-effects-lib.git libfe
gitdepend https://github.com/irov/math.git libmath
gitdepend https://github.com/akheron/jansson.git jansson

gitdependr https://github.com/glennrp/libpng.git libpng libpng17
gitdependr https://github.com/xiph/theora.git libtheora theora-1.1
gitdependr https://github.com/kcat/openal-soft.git openal-soft v1.18
gitdependr https://github.com/python/cpython.git Python 2.7
gitdependr https://github.com/python/cpython.git Python3 v3.6.5

svndepend svn://svn.code.sf.net/p/polyclipping/code/trunk/cpp polyclipping

echo Done
