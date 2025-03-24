
PROJECT_NAME := engine
PROJECT_TYPE := exec
PROJECT_DEPENDENCIES := lib/glad lib/tinyxml2

COMPILER := CXX

EXTRA_BUILD_FLAGS := -fPIC -I ../lib/glad -I ../lib/stbi $(shell pkg-config --cflags freetype2) $(shell pkg-config --cflags glfw3) $(shell pkg-config --cflags assimp) -std=c++17 -D_POSIX_C_SOURCE=200112L
EXTRA_LINK_FLAGS := $(shell pkg-config --libs freetype2) $(shell pkg-config --static --libs glfw3) $(shell pkg-config --static --libs assimp)

include $(BASE_DIR)/config.mk
include $(BASE_DIR)/$(SOURCE_DIR)/build.mk