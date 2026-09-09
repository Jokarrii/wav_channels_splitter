LINUX_SRCS = linux_srcs/main.cpp linux_srcs/Wav.cpp
WINDOWS_SRCS = windows_srcs/main.cpp windows_srcs/Wav.cpp

OBJS = $(LINUX_SRCS:.cpp=.o)

BIN_DIR = bin
NAME = $(BIN_DIR)/wav_channels_splitter
WIN_NAME = $(BIN_DIR)/wav_channels_splitter.exe

CXXFLAGS = -Wall -Wextra -Werror -std=c++20 -finput-charset=UTF-8 -fexec-charset=UTF-8
CXXFLAGS_WIN = -Wall -Wextra -Werror -std=c++20 -finput-charset=UTF-8 -fexec-charset=UTF-8 -municode

CXX = g++
WIN_CXX = x86_64-w64-mingw32-g++

RM = rm -f

all: $(NAME)

$(NAME): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

windows: | $(BIN_DIR)
	$(WIN_CXX) $(CXXFLAGS_WIN) -static -static-libgcc -static-libstdc++ -o $(WIN_NAME) $(WINDOWS_SRCS)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME) $(WIN_NAME)
	rmdir $(BIN_DIR) 2>/dev/null || true

re: fclean all
