#all:
#	g++ -std=c++11 ./main.cpp ./animator.cpp ./event.cpp ./path.cpp -o main -lsfml-graphics -lsfml-window -lsfml-system -lpthread -g -Wno-pmf-conversions -fsanitize=address
CPP_FILES := $(wildcard *.cpp)
OBJ_FILES := $(addprefix build/,$(notdir $(CPP_FILES:.cpp=.o)))
CC_FLAGS   = -g -Wno-pmf-conversions -fsanitize=address -std=c++11
CC         = g++
EXECUTABLE = main

all: $(EXECUTABLE)

main: $(OBJ_FILES)
	$(CC) $(LD_FLAGS) -o $@ $^ -lasan -lsfml-graphics -lsfml-window -lsfml-system -lpthread -lm

build/%.o: %.cpp
	$(CC) $(CC_FLAGS) -c -o $@ $<

clean:
	rm main ./build/* -f
