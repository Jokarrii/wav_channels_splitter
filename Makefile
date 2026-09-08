SRCS = main.cpp Wav.cpp

OBJS = $(SRCS:.cpp=.o)
NAME = wav_canal_splitter
WIN_NAME = wav_canal_splitter.exe

CXXFLAGS = -Wall -Wextra -Werror -std=c++20

CXX = g++
WIN_CXX = x86_64-w64-mingw32-g++

RM = rm -f

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

windows:
	$(WIN_CXX) $(CXXFLAGS) -static -static-libgcc -static-libstdc++ -o $(WIN_NAME) $(SRCS)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME) $(WIN_NAME)

re: fclean all
