#ifndef WAV_HPP
#define WAV_HPP

#include <iostream>
#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <cstring>
#include <iomanip>
#include <filesystem>

class Wav
{
public:

	enum class SampleFormat
	{
		PCM_U8,
		PCM_S16,
		PCM_S24,
		PCM_S32,
		FLOAT32,
		FLOAT64,
		UNKNOWN
	};

/*-------------- builder -----------------*/
	explicit Wav(const std::filesystem::path& filename);

/*---------------- getter -----------------*/
	uint32_t	fileSize() const;
	uint16_t	audioFormat() const;
	uint16_t	channels() const;
	uint32_t	sampleRate() const;
	uint32_t	byteRate() const;
	uint16_t	blockAlign() const;
	uint16_t	bitsPerSample() const;
	uint32_t	dataSize() const;

/*-------------- time info ---------------*/
	uint32_t	sampleCount() const;
	uint32_t	framesCount() const;

	void splitChannels(const std::filesystem::path& outputPrefix) const;
private:

	static bool checkID(std::ifstream& file, const char expected[4]);
	bool readChunkHeader(std::ifstream& file, char id[4], uint32_t& size);
	void readChunks(std::ifstream& file);
	void readFmtChunk(std::ifstream& file, uint32_t chunkSize);
	void readDataChunk(std::ifstream& file, uint32_t chunkSize);
	SampleFormat determineFormat() const;

	uint32_t				_fileSize{};
	uint16_t				_audioFormat{};
	uint16_t				_channels{};
	uint32_t				_sampleRate{};
	uint32_t				_byteRate{};
	uint16_t				_blockAlign{};
	uint16_t				_bitsPerSample{};
	SampleFormat			_format{SampleFormat::UNKNOWN};
	uint32_t				_dataSize{};
	std::vector<std::byte>	_data;
};

#endif
