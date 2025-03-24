.DEFAULT_GOAL := $(OUTPUT_FILENAME) 

src := $(shell find . -name '*.$(FILE_EXT.$(COMPILER))')
obj := $(patsubst ./%.$(FILE_EXT.$(COMPILER)), $(BUILD_DIR)/$(PROJECT_NAME)/%.o, $(src))

$(obj): $(BUILD_DIR)/$(PROJECT_NAME)/%.o : %.$(FILE_EXT.$(COMPILER))
	@echo "[$(COMPILER)] $(patsubst $(BUILD_DIR)/$(PROJECT_NAME)/%.o, %.$(FILE_EXT.$(COMPILER)), $@) -> $(@F)" && \
	mkdir -p $(dir $@) && \
	$($(COMPILER)) $(BUILD_FLAGS) $(patsubst $(BUILD_DIR)/$(PROJECT_NAME)/%.o, %.$(FILE_EXT.$(COMPILER)), $@) -o $@


$(OUTPUT_FILENAME): $(obj) $(PROJECT_TYPE)


.PHONY: static
static:
	@echo "[AR] $(OUTPUT_FILENAME)" && \
	$(AR) rcs $(OUT_DIR)/$(OUTPUT_FILENAME) $(obj)

.PHONY: shared
shared:
	@echo "[LD] $(OUTPUT_FILENAME)" && \
	$(LD) $(LINK_FLAGS) -shared $(obj) -o $(OUT_DIR)/$(OUTPUT_FILENAME)


.PHONY: exec
exec:
	@echo "[LD] $(OUTPUT_FILENAME)" && \
	$(LD) $(LINK_FLAGS) $(obj) $(OUT_DIR)/glad.a $(OUT_DIR)/tinyxml2.a -o $(OUT_DIR)/$(OUTPUT_FILENAME)
