SOURCE_DIR = src
BUILD_DIR  = build
GENERATOR = "MinGW Makefiles"

all:

ifeq ($(OS), Windows_NT)
	cmake -S $(SOURCE_DIR) -B $(BUILD_DIR) -G $(GENERATOR) 
else 
	cmake -S $(SOURCE_DIR) -B $(BUILD_DIR)
endif

	cmake --build $(BUILD_DIR)
