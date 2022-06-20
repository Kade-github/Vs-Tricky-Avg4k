/* RageSoundReader_Vorbisfile - Read from a Vorbisfile interface. */


#include "includes.h"

using OggVorbis_File = struct OggVorbis_File;

class Vorbisfile
{
public:
	OpenResult Open(RageFileBasic* pFile) override;

	int GetLength() const override;
	int SetPosition(int iFrame) override;
	int Read(float* pBuf, int iFrames) override;
	int GetSampleRate() const override;
	unsigned GetNumChannels() const override { return channels; }
	int GetNextSourceFrame() const override;
	RageSoundReader_Vorbisfile();
	~RageSoundReader_Vorbisfile() override;
	RageSoundReader_Vorbisfile* Copy() const override;

private:
	OggVorbis_File* vf;
	bool eof = false;
	bool FillBuf();
	std::string filename;
	int read_offset = 0;
	unsigned channels = 0;
};

#endif
