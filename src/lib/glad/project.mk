PROJECT_NAME := glad
PROJECT_TYPE := static
PROJECT_DEPENDENCIES := 

COMPILER := CC
EXTRA_BUILD_FLAGS := -fPIC  -std=c99

include $(BASE_DIR)/config.mk
include $(BASE_DIR)/$(SOURCE_DIR)/build.mk