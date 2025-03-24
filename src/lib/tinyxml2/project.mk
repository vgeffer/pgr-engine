PROJECT_NAME := tinyxml2
PROJECT_TYPE := static
PROJECT_DEPENDENCIES :=

COMPILER := CXX
EXTRA_BUILD_FLAGS := -fPIC 

include $(BASE_DIR)/config.mk
include $(BASE_DIR)/$(SOURCE_DIR)/build.mk