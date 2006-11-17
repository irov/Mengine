#include "MNG.h"

int main()
{
	/*	write	mng	*/
	mnglib::mngWriter		FORMAT;
	FORMAT.createMNG("output.mng");
	FORMAT.writeFrame(0,1);
	FORMAT.writeFrame(1,1);
	FORMAT.writeFrame(2,1);
	FORMAT.writeFrame(3,1);
	FORMAT.writeFrame(4,1);
	FORMAT.writeFrame(5,1);
	FORMAT.writeFrame(6,1);
	FORMAT.writeFrame(7,1);
	FORMAT.writeFrame(8,1);
	FORMAT.writeFrame(9,1);
	FORMAT.writeFrame(10,1);
	FORMAT.writeFrame(11,1);
	FORMAT.writeImageFromFile("Right_001.png");
	FORMAT.writeImageFromFile("Right_002.png");
	FORMAT.writeImageFromFile("Right_003.png");
	FORMAT.writeImageFromFile("Right_004.png");
	FORMAT.writeImageFromFile("Right_005.png");
	FORMAT.writeImageFromFile("Right_006.png");
	FORMAT.writeImageFromFile("Right_007.png");
	FORMAT.writeImageFromFile("Right_008.png");
	FORMAT.writeImageFromFile("Right_009.png");
	FORMAT.writeImageFromFile("Right_010.png");
	FORMAT.writeImageFromFile("Right_011.png");
	FORMAT.writeImageFromFile("Right_012.png");
	FORMAT.closeMNG();
};