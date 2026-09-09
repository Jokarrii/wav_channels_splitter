
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include "Wav.hpp"

bool	is_valid_path_wav(std::string const & input)
{
	std::filesystem::path path = input;

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
	if (!input.ends_with(".wav"))
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

void canal_splitter(Wav const &wav, const std::string& filename)
{
	std::filesystem::path inputPath(filename);

	std::filesystem::path outputPrefix = inputPath.parent_path() / inputPath.stem();

	wav.splitChannels(outputPrefix);
}

int	main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr	<< "Wrong number of argument." << std::endl
					<< "Usage 2: ./wav_channels_splitter <wav_file>" << std::endl;
		return (1);
	}
	std::string filename;
	if (!set_filename(filename, argv))
		return (1);
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
