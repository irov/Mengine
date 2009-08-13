# pragma  once

#	include <vector>
#	include <string>
#	include <list>

typedef std::vector<std::string> TStringVector;

namespace Menge
{
	class ImageDecoder;
}

struct TImageFrame
{
	size_t width;
	size_t height;
	Menge::ImageDecoder* imageDecoder;
	std::string filename;

	class Sorter
	{
	public:
		bool operator()( const TImageFrame& _left, const TImageFrame& _right )
		{
			return _left.height > _right.height;
		}
	};
};

typedef std::list<TImageFrame> TImageFrameList;

TStringVector build( const std::string& _atlasName, const TStringVector& _images, size_t _atlas_max_size, size_t _image_max_size, bool _trim );
void addFrame( const TImageFrame& _frame, bool _alpha );
void dumpAtlas( TImageFrameList& _frameList, const std::string& _filename, bool _alpha );

class Atlas
{
public:
	Atlas( bool _alpha );

	size_t insertFrame( size_t _x, size_t _y, const TImageFrame& _frame, size_t& _move_y );
	void writeAtlas( const std::string& _filename );
	float getUseCoeff() const;
	size_t getMaxWidth() const;
	size_t getMaxHeight() const;

protected:
	size_t m_maxWidth;
	size_t m_maxHeight;
	bool m_alpha;
	size_t m_occupiedSquare;
	size_t m_square;

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
