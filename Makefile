SOURCE_DIR = src
BUILD_DIR  = build

program:
	cmake -S $(SOURCE_DIR) -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
