
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <cwctype>
#include "Wav.hpp"

bool is_valid_path_wav(const std::filesystem::path& path)
{
	if (!std::filesystem::exists(path))
	{
		std::cerr << "The path does not exist." << std::endl;
		return false;
	}

	if (!std::filesystem::is_regular_file(path))
	{
		std::cerr << "The path is not a file." << std::endl;
		return false;
	}

	std::wstring ext = path.extension().wstring();

	for (wchar_t& c : ext)
		c = std::towlower(c);

	if (ext != L".wav")
	{
		std::cerr << "Input is not a .wav file" << std::endl;
		return false;
	}
	return true;
}

bool set_filename(std::string & filename, char **argv)
{
	if (is_valid_path_wav(argv[1]))
	{
		filename = argv[1];
		return true;
	}
	else
	{
		std::cerr << "unvalid path" << std::endl;
		return false;
	}
}

void canal_splitter(Wav const & wav, std::filesystem::path const & filename)
{
	std::filesystem::path outputPrefix = filename.parent_path() / filename.stem();

	wav.splitChannels(outputPrefix);
}

int wmain(int argc, wchar_t **argv)
{
	if (argc != 2)
	{
		std::wcerr	<< L"Wrong number of argument." << std::endl
					<< L"Usage: wav_channels_splitter.exe <wav_file>" << std::endl;
		return (1);
	}

	std::filesystem::path filename = argv[1];

	try
	{
		Wav wav(filename);
		canal_splitter(wav, filename);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}

	return (0);
}
