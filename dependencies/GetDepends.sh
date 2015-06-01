getdepend()
{
	if [ ! -f $2 ]; then
        ftp $1
    fi

	rm -rf $4
	7z x -y $2
	mv $3 $4
}

getdepend_tgz()
{
	if [ ! -f $2 ]; then
        ftp $1
    fi

	rm -rf $4
	tar -zxf $2
	mv $3 $4
}

svndepend()
{
    svn checkout -q $1 $2
}

getdepend http://zlib.net/zlib128.zip ./zlib128.zip zlib-1.2.8 zlib