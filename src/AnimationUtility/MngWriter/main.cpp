#include "MNG.h"

/*
struct MNGReader  
{
	void	open(const std::string& filename)
	{
		test = fopen(filename.c_str(),"rb");
	}

	void	close()
	{
		fclose(test);
	}

	bool	readImageSize(int& width, int&	height)
	{
		test = fopen("output.mng","rb");
		fread(&width,4,1,test);
		fread(&height,4,1,test);
		return	true;
	}

	int		readNumSprites()
	{
		int ns = -1;
		fread(&ns,4,1,test);
		return	ns;
	}

	bool	readDelayIndex(int& delay, int& index)
	{
		fread(&delay,4,1,test);
		fread(&index,4,1,test);
		return	true;
	}

	int	readNumImages()
	{
		int ns = -1;
		fread(&ns,4,1,test);
		return	ns;
	}

	void	readOffsetXY(int& offX, int& offY)
	{
		fread(&offX,4,1,test);
		fread(&offY,4,1,test);

	}

	bool	readBuffer(int& size, unsigned char* &buffer)
	{
		fread(&size,4,1,test);
		buffer = new unsigned char[size];
		fread(buffer,1,size,test);
		return	true;
	}

	FILE*	test;
};
*/
int main()
{
	/*	write	mng	*/
	mnglib::mngWriter		FORMAT;
	FORMAT.createMNG("output.mng");
	FORMAT.writeFrame(0,1);
	FORMAT.writeFrame(1,5);
	FORMAT.writeFrame(2,2);
	FORMAT.writeFrame(1,1);
	FORMAT.writeImageFromFile("font_z.png");
	FORMAT.writeImageFromFile("DOM.png");
	FORMAT.writeImageFromFile("met.png");
	FORMAT.closeMNG();
	/*
		исходящий файл будет содержать
		w,h,2
		67,10
		1,20
		2
		offX,offY,буфер с пнг(урезанная по альфе), размер буфера
		offX,offY,буфер с пнг(урезанная по альфе), размер буфера
	*/
	
	/*MNGReader	READER;
	READER.open("output.mng");
	int	width,	height;
	READER.readImageSize(width,height);
	int num = READER.readNumSprites();

	int	delay;
	int	index;
	for (int i =0; i < num; i++)
	{
		READER.readDelayIndex(delay,index);
	}

	num = READER.readNumImages();

	int	offX,offY,sizeBuf;
	char	name[20];
	unsigned	char*	buffer = NULL;

	for (int i =0; i < num; i++)
	{
		READER.readOffsetXY(offX,offY);
		READER.readBuffer(sizeBuf,buffer);

		itoa(i,name,9);
		std::string	filename(name);
		filename+=".png";

		FILE*	writeFileForTest = fopen(filename.c_str(),"wb");
		fwrite(buffer,1,sizeBuf,writeFileForTest);
		fclose(writeFileForTest);
		delete buffer;
	}
	READER.close();
	*/
};