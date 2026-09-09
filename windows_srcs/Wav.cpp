#include "Wav.hpp"

/*-------------- builder -----------------*/

Wav::Wav(const std::filesystem::path& filename)
{
	std::ifstream file(filename, std::ios::binary);
	if (!file)
		throw std::runtime_error("wav file can't be opened");
	if (!checkID(file, "RIFF"))
		throw std::runtime_error("Unsupported file format (RIFF chunk expected)");
	file.read(reinterpret_cast<char*>(&_fileSize), sizeof(_fileSize));
	if (!checkID(file, "WAVE"))
		throw std::runtime_error("Unsupported file format (WAVE chunk expected)");
	readChunks(file);
}

/*---------------- getter -----------------*/

uint32_t Wav::fileSize() const
{
	return (_fileSize);
}

uint16_t Wav::audioFormat() const
{
	return (_audioFormat);
}

uint16_t Wav::channels() const
{
	return (_channels);
}

uint32_t Wav::sampleRate() const
{
	return (_sampleRate);
}

uint32_t Wav::byteRate() const
{
	return (_byteRate);
}

uint16_t Wav::blockAlign() const
{
	return (_blockAlign);
}

uint16_t Wav::bitsPerSample() const
{
	return (_bitsPerSample);
}

uint32_t Wav::dataSize() const
{
	return (_dataSize);
}

uint32_t Wav::sampleCount() const
{
	return (_dataSize / (_bitsPerSample / 8));
}

uint32_t Wav::framesCount() const
{
	return (sampleCount() / _channels);
}

static void writeLE16(std::ofstream& file, uint16_t value)
{
	file.put(static_cast<char>(value & 0xFF));
	file.put(static_cast<char>((value >> 8) & 0xFF));
}

static void writeLE32(std::ofstream& file, uint32_t value)
{
	file.put(static_cast<char>(value & 0xFF));
	file.put(static_cast<char>((value >> 8) & 0xFF));
	file.put(static_cast<char>((value >> 16) & 0xFF));
	file.put(static_cast<char>((value >> 24) & 0xFF));
}

void Wav::splitChannels(const std::filesystem::path& outputPrefix) const
{
	if (_channels <= 1)
		return;

	const uint32_t monoDataSize = framesCount() * _bitsPerSample / 8;

	for (uint16_t channel = 0; channel < _channels; ++channel)
	{
		std::filesystem::path outputFile = outputPrefix.string() + "_ch" + std::to_string(channel + 1) + ".wav";

		std::ofstream file(outputFile, std::ios::binary);
		if (!file)
			throw std::runtime_error("Can't create output WAV file: " + outputFile.string());

		const uint32_t riffSize = 36 + monoDataSize;
		const uint16_t monoChannels = 1;
		const uint16_t monoBlockAlign = _bitsPerSample / 8;
		const uint32_t monoByteRate = _sampleRate * monoBlockAlign;

		/* RIFF */
		file.write("RIFF", 4);
		writeLE32(file, riffSize);

		/* WAVE */
		file.write("WAVE", 4);

		/* fmt */
		file.write("fmt ", 4);
		writeLE32(file, 16);
		writeLE16(file, _audioFormat);
		writeLE16(file, monoChannels);
		writeLE32(file, _sampleRate);
		writeLE32(file, monoByteRate);
		writeLE16(file, monoBlockAlign);
		writeLE16(file, _bitsPerSample);

		/* data */
		file.write("data", 4);
		writeLE32(file, monoDataSize);

		for (uint32_t frame = 0; frame < framesCount(); ++frame)
		{
			const std::size_t offset =
				static_cast<std::size_t>(frame) * _blockAlign
				+ static_cast<std::size_t>(channel) * _bitsPerSample / 8;

			file.write(
				reinterpret_cast<const char*>(_data.data() + offset),
				_bitsPerSample / 8
			);
		}

		if (!file)
			throw std::runtime_error("Error while writing output WAV file");
	}
}

/*---------------- member fonction -----------------*/

Wav::SampleFormat Wav::determineFormat() const
{
	switch (_audioFormat)
	{
		case 1:

			switch (_bitsPerSample)
			{
				case 8:  return SampleFormat::PCM_U8;
				case 16: return SampleFormat::PCM_S16;
				case 24: return SampleFormat::PCM_S24;
				case 32: return SampleFormat::PCM_S32;
				default: return SampleFormat::UNKNOWN;
			}

		case 3:

			switch (_bitsPerSample)
			{
				case 32: return SampleFormat::FLOAT32;
				case 64: return SampleFormat::FLOAT64;
				default: return SampleFormat::UNKNOWN;
			}

		default:
			return SampleFormat::UNKNOWN;
	}
}

bool Wav::checkID(std::ifstream& file, const char expected[4])
{
	char id[4];

	file.read(id, 4);
	if (!file)
		return false;
	return (std::memcmp(id, expected, 4) == 0);
}

bool Wav::readChunkHeader(std::ifstream& file, char id[4], uint32_t& size)
{
	file.read(id, 4);
	if (!file)
		return false;
	file.read(reinterpret_cast<char*>(&size), sizeof(size));
	if (!file)
		return false;
	return true;
}

void Wav::readChunks(std::ifstream& file)
{
	char		chunkID[4];
	uint32_t	chunkSize;

	while (readChunkHeader(file, chunkID, chunkSize))
	{
		if (memcmp(chunkID, "fmt ", 4) == 0)
			readFmtChunk(file, chunkSize);
		else if (memcmp(chunkID, "data", 4) == 0)
			readDataChunk(file, chunkSize);
		else
			file.seekg(chunkSize, std::ios::cur);

		if (chunkSize % 2 != 0)
			file.seekg(1, std::ios::cur);
	}
}

void Wav::readFmtChunk(std::ifstream& file, uint32_t chunkSize)
{
	if (chunkSize < 16)
		throw std::runtime_error("Unsupported file format (fmt chunk size invalid)");
	file.read(reinterpret_cast<char*>(&_audioFormat), sizeof(_audioFormat));
	file.read(reinterpret_cast<char*>(&_channels), sizeof(_channels));
	file.read(reinterpret_cast<char*>(&_sampleRate), sizeof(_sampleRate));
	file.read(reinterpret_cast<char*>(&_byteRate), sizeof(_byteRate));
	file.read(reinterpret_cast<char*>(&_blockAlign), sizeof(_blockAlign));
	file.read(reinterpret_cast<char*>(&_bitsPerSample), sizeof(_bitsPerSample));
	if (!file)
		throw std::runtime_error("Error while reading fmt chunk");
	_format = determineFormat();
	if (chunkSize > 16)
		file.seekg(chunkSize - 16, std::ios::cur);
}

void Wav::readDataChunk(std::ifstream& file, uint32_t chunkSize)
{
	_dataSize = chunkSize;
	_data.resize(chunkSize);
	file.read(reinterpret_cast<char*>(_data.data()), chunkSize);
	if (!file)
		throw std::runtime_error("Error while reading data chunk");
}
