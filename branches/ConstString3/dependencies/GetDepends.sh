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

getdepend http://www.ijg.org/files/jpegsr8c.zip jpegsr8c.zip jpeg-8c libjpeg
getdepend http://prdownloads.sourceforge.net/libpng/lpng157.zip lpng157.zip lpng157 libpng
getdepend http://downloads.xiph.org/releases/ogg/libogg-1.3.0.zip libogg-1.3.0.zip libogg-1.3.0 ogg
getdepend http://netcologne.dl.sourceforge.net/project/pybind/pybind-1.2.zip pybind-1.2.zip pybind-1.2 pybind
getdepend_tgz http://www.python.org/ftp/python/2.7.2/Python-2.7.2.tgz Python-2.7.2.tgz Python-2.7.2 Python
getdepend http://downloads.xiph.org/releases/vorbis/libvorbis-1.3.2.zip libvorbis-1.3.2.zip libvorbis-1.3.2 vorbis
getdepend http://zlib.net/zlib125.zip zlib125.zip zlib-1.2.5 zlib
getdepend_tgz http://freefr.dl.sourceforge.net/project/boost/boost/1.48.0/boost_1_48_0.tar.gz boost_1_48_0.tar.gz boost_1_48_0 boost 
