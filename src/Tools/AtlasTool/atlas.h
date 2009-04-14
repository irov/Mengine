# pragma  once

#	include <vector>
#	include <string>
#	include <list>

typedef std::vector<std::string> TStringVector;

namespace Menge
{
	class ImageDecoderInterface;
}

struct TImageFrame
{
	size_t width;
	size_t height;
	Menge::ImageDecoderInterface* imageDecoder;
	std::string filename;
};

typedef std::list<TImageFrame> TImageFrameList;

TStringVector build( const std::string& _atlasName, const TStringVector& _images, size_t _atlas_max_size, size_t _image_max_size );
void addFrame( const TImageFrame& _frame, bool _alpha );
void dumpAtlas( TImageFrameList& _frameList, const std::string& _filename, bool _alpha );

class Atlas
{
public:
	Atlas( bool _alpha );

	size_t insertFrame( size_t _x, size_t _y, const TImageFrame& _frame, size_t& _move_y );
	void writeAtlas( const std::string& _filename );

protected:
	size_t m_maxWidth;
	size_t m_maxHeight;
	bool m_alpha;

	struct TAtlasFrame
	{
		size_t left;
		size_t top;
		size_t right;
		size_t bottom;
		TImageFrame imageFrame;
	};
	std::vector<TAtlasFrame> m_frames;
	std::string m_filename;

	bool intersect( const TAtlasFrame& _frame1, const TAtlasFrame& _frame2 );
};
