$SOURCE_DIR = "."
$BUILD_DIR = "../build/"

mkdir -p $BUILD_DIR

cmake -S $SOURCE_DIR -B $BUILD_DIR -G "MinGW Makefiles"
cmake --build $BUILD_DIR
