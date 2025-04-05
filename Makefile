.DEFAULT_GOAL := all

# Dirs
BASE_DIR := $(strip $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST)))))
SOURCE_DIR := $(strip $(shell dirname $(shell find -name 'build.mk')))


projects := $(shell realpath --relative-to=$(SOURCE_DIR) $(strip $(shell dirname $(shell find -name 'project.mk'))))
project_builds := 



$(projects):
	make -C $(SOURCE_DIR)/$@ -f project.mk BASE_DIR=$(BASE_DIR) PROJECT=$@ OUT_DIR=$(BASE_DIR) BUILD_DIR=$(BASE_DIR)/.tmp SOURCE_DIR=$(SOURCE_DIR)

.PHONY: all
all: $(projects)

.PHONY: clean
clean:
	@rm -rfv $(BASE_DIR)/.tmp
	@rm -fv $(BASE_DIR)/*.a
	@rm -fv $(BASE_DIR)/*.so
	@rm -fv $(BASE_DIR)/*.x86_64
	@rm -fv $(BASE_DIR)/*.i386

.PHONY: distclean
distclean:
