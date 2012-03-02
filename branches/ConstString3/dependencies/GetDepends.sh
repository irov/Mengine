getdepend()
{
	if test -s $2
	then echo "already download" $2
	else ftp $1
	fi

	rm -rf $4
	unzip -o -q $2
	mv $3 $4
}

getdepend_tgz()
{
	if test -s $2
	then echo "already download" $2
	else ftp $1
	fi

	rm -rf $4
	tar -zxf $2
	mv $3 $4
}

###################	GLee
if test -s GLee-5.4.0-src.tar
then echo "already download" GLee-5.4.0-src.tar
else 
	ftp http://elf-stone.com/downloads/GLee/GLee-5.4.0-src.tar.gz	
	gzip -d -f GLee-5.4.0-src.tar.gz
fi

rm -rf Glee

mkdir GLee
mv GLee-5.4.0-src.tar GLee/GLee-5.4.0-src.tar

cd GLee
tar -xf GLee-5.4.0-src.tar
mv GLee-5.4.0-src.tar ../GLee-5.4.0-src.tar
cd ..

###################	BOX2D
if test -s Box2D_v2.2.1.zip
then echo "already download" Box2D_v2.2.1.zip
else 
	ftp http://box2d.googlecode.com/files/Box2D_v2.2.1.zip	
fi

rm -rf box2d

mkdir box2d
mv Box2D_v2.2.1.zip box2d/Box2D_v2.2.1.zip

cd box2d
unzip -o -q Box2D_v2.2.1.zip
mv Box2D_v2.2.1.zip ../Box2D_v2.2.1.zip
cd ..



getdepend http://www.ijg.org/files/jpegsr8d.zip jpegsr8d.zip jpeg-8d libjpeg
getdepend http://prdownloads.sourceforge.net/libpng/lpng159.zip lpng159.zip lpng159 libpng
getdepend http://downloads.xiph.org/releases/ogg/libogg-1.3.0.zip libogg-1.3.0.zip libogg-1.3.0 ogg
getdepend http://netcologne.dl.sourceforge.net/project/pybind/pybind-1.2.zip pybind-1.2.zip pybind-1.2 pybind
getdepend_tgz http://www.python.org/ftp/python/2.7.2/Python-2.7.2.tgz Python-2.7.2.tgz Python-2.7.2 Python
getdepend http://downloads.xiph.org/releases/vorbis/libvorbis-1.3.2.zip libvorbis-1.3.2.zip libvorbis-1.3.2 vorbis
getdepend http://zlib.net/zlib126.zip zlib126.zip zlib-1.2.6 zlib
getdepend_tgz http://freefr.dl.sourceforge.net/project/boost/boost/1.49.0/boost_1_49_0.tar.gz boost_1_49_0.tar.gz boost_1_49_0 boost 
