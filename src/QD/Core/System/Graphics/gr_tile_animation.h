#ifndef __GR_TILE_ANIMATION_H__
#define __GR_TILE_ANIMATION_H__

#include "gr_tile_sprite.h"

typedef void (*CompressionProgressHandler)(int percents_loaded, void* context);

class grTileAnimation
{
public:
	grTileAnimation();

	bool isEmpty() const { return !frameCount_; }

	void clear();

	int frameCount() const { return frameCount_; }
	const Vect2i& frameSize() const { return frameSize_; }
	const Vect2i& frameTileSize() const { return frameTileSize_; }
	int tileCount() const { return tileOffsets_.size() - 1; }

	void init(int frame_count, const Vect2i& frame_size, bool alpha_flag);

	void compact();

	bool compress(grTileCompressionMethod method);

	grTileSprite getTile(int tile_index) const;

	void addFrame(const unsigned* frame_data);

	bool save(XStream& fh) const;
	bool load(XStream& fh);
	bool load(XZipStream& fh);
    
	void drawFrame(const Vect2i& position, int frame_index, int mode = 0) const;
	void drawFrame(const Vect2i& position, int frame_index, float angle, int mode = 0) const;

	static void setProgressHandler(CompressionProgressHandler handler, void* context){
		progressHandler_ = handler;
		progressHandlerContext_ = context;
	}

private:

	grTileCompressionMethod compression_;

	/// true ���� ���� �����-�����
	bool hasAlpha_;

	/// ������� ����� � ��������
	/// ����� ���� �������������� �� ������� �����
	Vect2i frameSize_;

	/// ������� ����� � ������
	Vect2i frameTileSize_;

	/// ���������� ������
	int frameCount_;

	typedef std::vector<unsigned> FrameIndex;
	/// ������ ������ - ������ ������, �� ������� ������� �����
	/// frameTileSize_.x * frameTileSize_.y �� ����
	FrameIndex frameIndex_;

	typedef std::vector<unsigned> TileOffsets;
	/// �������� �� ������ ������� �����
	TileOffsets tileOffsets_;
	typedef std::vector<unsigned> TileData;
	/// ������ ������
	TileData tileData_;

	static CompressionProgressHandler progressHandler_;
	static void* progressHandlerContext_;
};

#endif /* __GR_TILE_ANIMATION_H__ */
