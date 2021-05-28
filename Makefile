####################################################################################################
#                                  Configuration variables                                         #
####################################################################################################
PROJECT_NAME	:= STM32_SLEEPY_SENSOR

# Version stuff
SW_VER := $(subst .,_,$(shell sVersion -l))
VERSION := $(SW_VER)
export VERSION

PTLD := $(shell cygpath -u $(PTLD) )

MAKE_FLAGS := --no-print-directory \
			  --no-builtin-rules \
			  --silent \


# Compiler Tools
GCOVR_EXE := gcovr
ARM		  := arm-none-eabi
CC		  := $(ARM)-gcc
LD		  := $(ARM)-gcc
OBJCOPY	  := $(ARM)-objcopy
AS        := $(ARM)-as
AR        := $(ARM)-ar
NM        := $(ARM)-nm
STRIP     := $(ARM)-strip


STM32_SRC_DIRS           := Src Workspace
BUILD_OUT_DIR            := Build_output
STM32_LINKER_SCRIPT 	 := Workspace/linker_script/arm-gcc-link.ld
STM32_ELF_FILE           := $(BUILD_OUT_DIR)/$(PROJECT_NAME).elf
STM32_MAP_FILE  	     := $(BUILD_OUT_DIR)/$(PROJECT_NAME).map
STM32_COMPILER_OUTPUT    := $(BUILD_OUT_DIR)/compile_log.txt
STM32_MEM_OUTPUT_FILE    := $(BUILD_OUT_DIR)/memory_analysis.txt
RELEASE_PACKAGE_BASE_DIR := Release_package
RELEASE_PACKAGE_NAME     := $(RELEASE_PACKAGE_BASE_DIR)/$(PROJECT_NAME)_$(SW_VER)

#Find all the source file in the given directories
SRCS             := $(shell find $(STM32_SRC_DIRS) -type f -name '*.c')

#add the "-I" to all folders found by the find command :)
INCLUDES         := $(addprefix -I ,$(shell find $(STM32_SRC_DIRS) -type d))

VPATH            := $(dir $(SRCS))
TARGET_OBJ_DIR   := $(BUILD_OUT_DIR)/object_files
TARGET_OBJS      := $(patsubst %.c, $(TARGET_OBJ_DIR)/%.o, $(notdir $(SRCS)))
TARGET_DEPS      := $(TARGET_OBJS:.o=.d)
TARGET_EXEC      := $(TARGET_OBJ_DIR)/$(PROJECT_NAME).elf
AUTOVERSION_FILE := src/autoversion.h
MEM_STATS_HEADER := src/VER/MEM_STATS.h

ifeq ($(RELEASE),y)
	OPTIMISATION := -O3
else
	OPTIMISATION := -O0
endif

CFLAGS :=  \
	-mcpu=cortex-m3 \
	-mthumb \
	-Wall \
	-Werror=implicit-function-declaration \
	-fshort-enums \
	-fpack-struct=1 \
	-fstack-usage \
	-Wno-pragmas \
	-Wno-pointer-sign \
	-ffunction-sections \
	-Wno-format \
	-Wswitch-default \
	-g \
	$(OPTIMISATION) \
	-c \
	-Wl,--print-memory-usage \
	-MMD \
	-DSTM32DRIVERS \
	-DSTM32F103C8 \
	-DSTM32F10X_MD \
	-DUSE_STDPERIPH_DRIVER \
	-D__ASSEMBLY__ \

LDFLAGS := \
	-mcpu=cortex-m3 \
	-mthumb \
	-g \
	-nostartfiles \
	"-Wl,-Map=$(STM32_MAP_FILE)" \
	$(OPTIMISATION) \
	-Wl,--gc-sections \
	-L$(dir $(STM32_LINKER_SCRIPT)) \
	-T$(STM32_LINKER_SCRIPT) -g -o $(STM32_ELF_FILE) \


####################################################################################################


####################################################################################################
#                      All (Default) Target and other Build Targets                                #
####################################################################################################
.PHONY: all
all: $(TARGET_OBJ_DIR) timestamp_build_start autoversion $(TARGET_EXEC) memory_stats timestamp_build_stop
	@echo "--> All build files generated at $(realpath $(BUILD_OUT_DIR))..."
	@echo "--> Build Succesfully Completed..."

.PHONY: rebuild
rebuild: build_clean all

$(TARGET_OBJ_DIR)/%.o: %.c
	@rm -f $@
	@echo "--> Compiling $(notdir $@) ..."
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@ 2>&1 | tee -a $(STM32_COMPILER_OUTPUT)

$(TARGET_OBJ_DIR):
	@echo "--> Folder "$(TARGET_OBJ_DIR)" does not exist, creating it now"
	@mkdir -p $(TARGET_OBJ_DIR)
	
$(TARGET_EXEC): 
	@echo Compiler warnings generated by GCC for $(PROJECT_NAME) > $(STM32_COMPILER_OUTPUT)
	@echo ' ' >> $(STM32_COMPILER_OUTPUT)
	@$(MAKE) $(MAKE_FLAGS) $(TARGET_OBJS)
	@echo "--> Linking Object Files..."
	@$(LD) $(LDFLAGS) $(TARGET_OBJS)
	@$(OBJCOPY) -O binary $(STM32_ELF_FILE) $(BUILD_OUT_DIR)/$(PROJECT_NAME).bin
	@$(OBJCOPY) -O ihex $(STM32_ELF_FILE) $(BUILD_OUT_DIR)/$(PROJECT_NAME).hex
	
	
.PHONY: build_clean
build_clean:
	@rm -fr $(BUILD_OUT_DIR)
	@rm -fr $(AUTOVERSION_FILE)
	@echo "--> Build Cleaned..."

####################################################################################################
#                                  Ceedling test target & rules                                    #
####################################################################################################
# Various test reports
CEEDLING_GCOV_DIR					   := CodeCoverage
CEEDLING_TEST_XML_TEST_REPORT_ORIGIN   := test/build/artifacts/test
CEEDLING_GCOV_XML_TEST_REPORT_ORIGIN   := test/build/artifacts/gcov
CEEDLING_TEST_XML_TEST_REPORT_DEST 	   := $(CEEDLING_GCOV_DIR)/Test_Report
CEEDLING_LCOV_XML_TEST_REPORT_DEST	   := $(CEEDLING_GCOV_DIR)/LCOV

# Output files
GCOV_OUTPUT_DIR := test/build/gcov/out

# Files to be excluded from gcov coverage
UNWANTED_GEN_COVERAGE :=        \
$(GCOV_OUTPUT_DIR)/cmock*       \
$(GCOV_OUTPUT_DIR)/SELMATH*     \
$(GCOV_OUTPUT_DIR)/CHKSUM*      \
$(GCOV_OUTPUT_DIR)/STDC*        \
$(GCOV_OUTPUT_DIR)/test_*       \
$(GCOV_OUTPUT_DIR)/unity.*      \
$(GCOV_OUTPUT_DIR)/*helper*     \
$(GCOV_OUTPUT_DIR)/mock*

# Creates Ceedling environment if it does not exist
test/vendor:
	@createCeedlingEnv

# Wild card test will allow any test to run once the name is provided
%.test: test/vendor autoversion
	@$(eval TEST_FILE := $(subst .test,.c,$@))
	@$(eval MSN := $(subst test_,,$(TEST_FILE)))
	@echo Testing $(TEST_FILE)...
	@cd test && rake gcov:$(TEST_FILE) 2>&1 | \
	@sed -n -e '1,/GCOV: CODE COVERAGE SUMMARY/p; s/GCOV: CODE COVERAGE SUMMARY/---------------------------/p; /$(MSN) /p'

%.test_lcov: test_clean $(CEEDLING_LCOV_XML_TEST_REPORT_DEST) autoversion
	@$(eval TEST_FILE := $(subst .test_lcov,,$@))
	@$(MAKE) $(MAKE_FLAGS) $(TEST_FILE).test
	@rm -fr $(UNWANTED_GEN_COVERAGE)
	@echo ' '
	@echo "Generating HTML for Code Coverage on $(TEST_FILE)"
	@cd test && lcov -t 'Title of Test' -o output_file.info -c -d ..
	@cd test && genhtml -o $(CEEDLING_LCOV_XML_TEST_REPORT_DEST) output_file.info
	@echo "--> Launching HTML..."
	@start test/$(CEEDLING_LCOV_XML_TEST_REPORT_DEST)/index.html

.PHONY: test_all
test_all: test/vendor $(CEEDLING_TEST_XML_TEST_REPORT_DEST) autoversion
	@cd test && ceedling gcov:all
	@rm -fr $(UNWANTED_GEN_COVERAGE)

.PHONY: gen_lcov_report
gen_lcov_report: test_clean autoversion test_all 
	@echo "--> Generating LCOV reports.. Please be patient..."
	@cd test && lcov -t 'LCov_report' -o output_file.info -c -d .. --rc genhtml_hi_limit=95
	@cd test && genhtml -o $(CEEDLING_LCOV_XML_TEST_REPORT_DEST) output_file.info
	@cd test/$(CEEDLING_LCOV_XML_TEST_REPORT_DEST) && start index.html
#@$(call check_test_result)

# Cleans up any generated files from tests
.PHONY: test_clean
test_clean: 
	@-cd test && ceedling clobber
	@echo "--> Test Cleaned..."


# user fn check overall test result for Pass \ Fail.  Exits with error code on failure
define check_test_result
	@if grep -q "Overall Test Result: PASS" $(CEEDLING_TEST_XML_TEST_REPORT_DEST)SoftwareCeedlingTestReport.html; \
	then \
	  echo "--> Overall Test Result: PASS"; \
	else \
	  echo "--> Overall Test Result: FAIL"; \
	  exit 1; \
	fi
endef


$(CEEDLING_LCOV_XML_TEST_REPORT_DEST):
	mkdir -p $(CEEDLING_LCOV_XML_TEST_REPORT_DEST)

$(CEEDLING_TEST_XML_TEST_REPORT_DEST):
	mkdir -p $(CEEDLING_TEST_XML_TEST_REPORT_DEST)

####################################################################################################
#                                  Doxygen targets & rules                                         #
####################################################################################################
DOXYGEN_EXE    := doxygen.exe
DOXYGEN_OUTPUT := doc/doxygen

.PHONY: doxygen
doxygen:
	@echo "--> Running Doxygen..."
	@mkdir -p $(DOXYGEN_OUTPUT)
	@$(DOXYGEN_EXE) -b Tool_Cfg/Doxygen/Doxygen_config
	@cd $(DOXYGEN_OUTPUT) && find -type f \( -name '*.md5' -o -name '*.map' -o -name '*globals*' -o -name '*dir_*' \) -print0 | xargs -0 rm -f

# Cleans up any generated files from doxygen
.PHONY: doxygen_clean
doxygen_clean:
	@echo "--> Cleaning doxygen..."
	@rm -rf $(DOXYGEN_OUTPUT)/*



####################################################################################################
#                                  Miscellaneous targets & rules                                   #
####################################################################################################

# Print all targets if no target has been supplied
.PHONY: no_targets__ list
no_targets__:
list:
	@echo "--> Available Targets:"
	@sh -c "$(MAKE) -p no_targets__ | awk -F':' '/^[a-zA-Z0-9][^\$$#\/\\t=]*:([^=]|$$)/ {split(\$$1,A,/ /);for(i in A)print A[i]}' | grep -v '__\$$' | sort"


.PHONY: print_file_list
print_file_list:
ifeq ($(FILE),y)
	@$(eval OPT := /f )
else
	@$(eval OPT := )
endif
	@cmd //c "tree /a $(OPT)" | tee file_list.txt

autoversion:
	@echo "--> Generating autoversion header file..."
	@sVersion --autoversion

$(STM32_MAP_FILE):
	@echo "--> $(STM32_MAP_FILE) does not exist, generating now..."
	@$(MAKE) $(MAKE_FLAGS) all

.PHONY: total_clean
total_clean: build_clean test_clean
	@-rm -fR CodeCoverage
	@echo "--> Clean Complete.."

timestamp_build_start:
	@date +%s > $(BUILD_OUT_DIR)/temp_build_time.txt 2>&1

timestamp_build_stop:
	@date +%s >> $(BUILD_OUT_DIR)/temp_build_time.txt 2>&1
	@awk 'FNR==2{end=$$0}; FNR==1{start=$$0} END{ printf "--> Build time: " end - start " seconds"}' $(BUILD_OUT_DIR)/temp_build_time.txt
	@awk 'FNR==2{end=$$0}; FNR==1{start=$$0} END{ printf "Build time in seconds, %d", end - start }' $(BUILD_OUT_DIR)/temp_build_time.txt > $(BUILD_OUT_DIR)/build_time.csv
	@rm -fr $(BUILD_OUT_DIR)/temp_build_time.txt
	@echo ' '

# Script that gens a header file with %rom and %ram used
gen_memory_header:
	@echo "--> Rebuilding image with updated memory stats..."
	@gen_mem_stats $(STM32_MEM_OUTPUT_FILE) $(MEM_STATS_HEADER)


# Set memory limits to match the Resource Usage in the Software Integration Spec
# ROM limits
ROM_SIZE := $(shell awk '/rom/ { rom = strtonum($$9); exit; } END { print rom } ' $(STM32_LINKER_SCRIPT) )
ROM_PERC_LIMIT := 80
ROM_CALC_LIMIT := $(shell echo $(($(ROM_SIZE) * $(ROM_PERC_LIMIT) / 100 )))

# RAM limits
RAM_SIZE := $(shell awk '/ram/ { ram = strtonum($$9); exit; } END { print ram } ' $(STM32_LINKER_SCRIPT) )
RAM_PERC_LIMIT := 80
RAM_CALC_LIMIT := $(shell echo $$(($(RAM_SIZE) * $(RAM_PERC_LIMIT) / 100 )))


.PHONY: memory_stats
memory_stats: $(STM32_MAP_FILE)
	@echo "--> Analysing elf file for memory stats..."
	@echo "--> Output from GNU \"size\" tool.." | tee $(STM32_MEM_OUTPUT_FILE)
	@size -B $(STM32_ELF_FILE) | tee -a $(STM32_MEM_OUTPUT_FILE)

	@awk 'BEGIN{ printf "\n=======================================================\n\
	===================== RAM STATS =======================\n\
	=======================================================\n\n"; } \
	/$(PROJECT_NAME)/ { RamUsed = ($$2) + ($$3); } \
	END{ printf "Available Ram in Bytes: %-6d\nRam used in Bytes:      %-6d\n%%Ram used:              %-3.0f%\n", $(RAM_SIZE), RamUsed, ( RamUsed/$(RAM_SIZE) * 100 ); }' $(STM32_MEM_OUTPUT_FILE) | tee -a $(STM32_MEM_OUTPUT_FILE)

	@awk 'BEGIN{ printf "\n=======================================================\n\
	=================== FLASH STATS =======================\n\
	=======================================================\n\n"; } \
	/$(PROJECT_NAME)/ { FlashUsed = ($$1);} \
	END{ printf "Available Flash in Bytes: %-6d\nFlash used in Bytes:      %-6d\n%%Flash used:              %-3.0f%\n\
	-------------------------------------------------------\n\n", $(ROM_SIZE), FlashUsed, ( FlashUsed/$(ROM_SIZE) * 100 ); }' $(STM32_MEM_OUTPUT_FILE) | tee -a $(STM32_MEM_OUTPUT_FILE)
	
	@awk 'BEGIN{ printf "\n=======================================================\n\
	===================== NVM STATS =======================\n\
	=======================================================\n\n"; } \
	/NVM_info_s/ { NVM_used = strtonum($$2); exit; } \
	END{ printf "NVM used in Bytes: %-6d\n\
	-------------------------------------------------------\n\n", NVM_used; }' $(STM32_MAP_FILE) | tee -a $(STM32_MEM_OUTPUT_FILE)
	@echo "--> Output file \"$(STM32_MEM_OUTPUT_FILE)\" created"

	
.PHONY: test
test:
	@echo $(PTLD)
	

.PHONY: release_package
release_package:
	@echo "--> Creating release package $(SW_VER)..."
	@-rm -fr $(RELEASE_PACKAGE_BASE_DIR)
	@mkdir -p $(RELEASE_PACKAGE_NAME)
	@$(MAKE) $(MAKE_FLAGS) rebuild
	@$(MAKE) $(MAKE_FLAGS) memory_stats
	#@$(MAKE) $(MAKE_FLAGS) gen_lcov_report
	@$(MAKE) $(MAKE_FLAGS) doxygen
	@gen_build_info $(RELEASE_PACKAGE_NAME)
	@echo "--> Copying goodies to $(RELEASE_PACKAGE_NAME) folder..."
	@cp -r $(BUILD_OUT_DIR)/. $(RELEASE_PACKAGE_NAME)
	@find $(RELEASE_PACKAGE_NAME)/object_files/ -type f -name *.d -delete -o -name *.su -delete
	@cp -r $(CEEDLING_GCOV_DIR) $(RELEASE_PACKAGE_NAME)
	@-cp -r $(DOXYGEN_OUTPUT) $(RELEASE_PACKAGE_NAME)
	@find src/ -type f | xargs wc -l | sort -r | sed '/total/d' > $(RELEASE_PACKAGE_NAME)/line_count.txt
	@find  $(RELEASE_PACKAGE_NAME) -type f -print0 | sort -z | xargs -0 sha1sum > $(RELEASE_PACKAGE_NAME)/chksum.txt
	@echo "--> Checksum file generated for $(RELEASE_PACKAGE_NAME)..."
	@echo "--> Zipping up the release package..."
	@cd $(RELEASE_PACKAGE_BASE_DIR) && 7za a "$(notdir $(RELEASE_PACKAGE_NAME)).zip" ./* > /dev/null
	@-rm -fr $(RELEASE_PACKAGE_NAME)
	@echo "--> Release package created $(SW_VER)..."

	

-include $(TARGET_DEPS)


