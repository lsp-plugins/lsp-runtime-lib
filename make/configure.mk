# Definitions
PREFIX                     := /usr/local
BASEDIR                    := $(CURDIR)
BUILDDIR                   := $(BASEDIR)/.build
CONFIG                     := $(BASEDIR)/.config.mk
MODULES                    := $(BASEDIR)/modules
TEST                       := 0
DEBUG                      := 0
PROFILE                    := 0
TRACE                      := 0

include $(BASEDIR)/make/system.mk
include $(BASEDIR)/project.mk
include $(BASEDIR)/make/tools.mk
include $(BASEDIR)/dependencies.mk

DEPENDENCIES               += $(TEST_DEPENDENCIES)

ifeq ($(findstring -devel,$(VERSION)),-devel)
  $(foreach dep, $(DEPENDENCIES), \
    $(eval $(dep)_BRANCH=devel) \
  )
  # Strip '-devel' from version
  tmp_version :=$(shell echo "$(VERSION)" | sed s/-devel//g)
  VERSION=$(tmp_version)
else
  $(foreach dep, $(DEPENDENCIES), \
    $(eval \
      $(dep)_BRANCH="$($(dep)_NAME)-$($(dep)_VERSION)" \
    ) \
  )
endif

define pkgconfig =
  $(eval name = $(1))
  $(if $($(name)_NAME), \
    $(if $($(name)_CFLAGS),,  $(eval $(name)_CFLAGS  := $(shell pkg-config --cflags "$($(name)_NAME)"))) \
  )
  $(if $($(name)_NAME), \
    $(if $($(name)_LDLAGS),,  $(eval $(name)_LDFLAGS := $(shell pkg-config --libs "$($(name)_NAME)"))) \
  )
  $(if $($(name)_OBJ),,     $(eval $(name)_OBJ     :=))
endef

define bldconfig =
  $(eval name = $(1))
  $(if $($(name)_CFLAGS),,  $(eval $(name)_CFLAGS  := "-I\"$($(name)_INC)\""$(xflags)))
  $(if $($(name)_LDLAGS),,  $(eval $(name)_LDFLAGS :=))
  $(if $($(name)_OBJ),,     $(eval $(name)_OBJ     := "$($(name)_BIN)/$($(name)_NAME).o"))
  $(if $($(name)_MFLAGS),, \
    $(eval $(name)_MFLAGS := \
      $(if $(patsubst $(ARTIFACT_NAME),,$($(name)_NAME)),"-DLSP_BUILTIN_MODULE -fvisibility=hidden",)) \
  )
endef

define vardef =
  $(eval name = $(1))
  # Override variables if they are not defined
  $(if $($(name)_NAME), \
    $(if $($(name)_PATH),,    $(eval $(name)_PATH    := $(MODULES)/$($(name)_NAME))) \
    $(if $($(name)_INC),,     $(eval $(name)_INC     := $($(name)_PATH)/include)) \
    $(if $($(name)_SRC),,     $(eval $(name)_SRC     := $($(name)_PATH)/src)) \
    $(if $($(name)_TEST),,    $(eval $(name)_TEST    := $($(name)_PATH)/test)) \
    $(if $($(name)_TESTING),, $(eval $(name)_TESTING := 0)) \
    $(if $($(name)_BIN),,     $(eval $(name)_BIN     := $(BUILDDIR)/$($(name)_NAME))) \
  )
  
  $(if $(findstring system,$($(name)_VERSION)), \
    $(eval $(call pkgconfig, $(name))), \
    $(eval $(call bldconfig, $(name))) \
  )
endef

# Define predefined variables
ifndef $(ARTIFACT_VARS)_NAME
  $(ARTIFACT_VARS)_NAME      := $(ARTIFACT_NAME)
endif
ifndef $(ARTIFACT_VARS)_VERSION 
  $(ARTIFACT_VARS)_VERSION   := $(VERSION)
endif
ifndef $(ARTIFACT_VARS)_PATH
  $(ARTIFACT_VARS)_PATH      := $(BASEDIR)
endif

$(ARTIFACT_VARS)_TESTING    = $(TEST)

OVERALL_DEPS := $(DEPENDENCIES) $(ARTIFACT_VARS)
__tmp := $(foreach dep,$(OVERALL_DEPS),$(call vardef, $(dep)))

CONFIG_VARS = \
  $(COMMON_VARS) \
  $(TOOL_VARS) \
  $(foreach name, $(OVERALL_DEPS), \
    $(name)_NAME \
    $(name)_VERSION \
    $(name)_BRANCH \
    $(name)_PATH \
    $(name)_INC \
    $(name)_SRC \
    $(name)_TEST \
    $(name)_TESTING \
    $(name)_URL \
    $(name)_BIN \
    $(name)_CFLAGS \
    $(name)_MFLAGS \
    $(name)_LDFLAGS \
    $(name)_OBJ \
  )

.DEFAULT_GOAL      := config
.PHONY: config prepare help
.PHONY: $(CONFIG_VARS)

prepare:
	@echo "Configuring build..."
	@echo "# Project settings" > "$(CONFIG)"

$(CONFIG_VARS): prepare
	@echo "$(@)=$($(@))" >> "$(CONFIG)"

config: $(CONFIG_VARS)
	@echo "Configured OK"

help: | toolvars sysvars
	@echo ""
	@echo "List of variables for each dependency:"
	@echo "  <ARTIFACT>_BIN            location to put all binaries when building artifact"
	@echo "  <ARTIFACT>_BRANCH         git branch used to checkout source code"
	@echo "  <ARTIFACT>_CFLAGS         C/C++ flags to access headers of the artifact"
	@echo "  <ARTIFACT>_INC            path to include files of the artifact"
	@echo "  <ARTIFACT>_LDFLAGS        linker flags to link with artifact"
	@echo "  <ARTIFACT>_MFLAGS         artifact-specific compilation flags"
	@echo "  <ARTIFACT>_NAME           the artifact name used in pathnames"
	@echo "  <ARTIFACT>_OBJ            path to output object file for artifact"
	@echo "  <ARTIFACT>_PATH           location of the source code of the artifact"
	@echo "  <ARTIFACT>_SRC            path to source code files of the artifact"
	@echo "  <ARTIFACT>_TEST           location of test files of the artifact"
	@echo "  <ARTIFACT>_URL            location of the artifact git repoisitory"
	@echo "  <ARTIFACT>_VERSION        version of the artifact used for building:"
	@echo "                              - version contained in git tag"
	@echo "                              - git branch name"
	@echo "                              - 'system' for system library use"
	@echo ""
	@echo "Artifacts used for build:"
	@echo "  $(DEPENDENCIES)"

