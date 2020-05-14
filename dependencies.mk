# Variables that describe dependencies
LSP_COMMON_LIB_VERSION     := 1.0.5
LSP_COMMON_LIB_NAME        := lsp-common-lib
LSP_COMMON_LIB_TYPE        := src
LSP_COMMON_LIB_URL         := https://github.com/sadko4u/$(LSP_COMMON_LIB_NAME).git

LSP_LLTL_LIB_VERSION       := 0.5.1
LSP_LLTL_LIB_NAME          := lsp-lltl-lib
LSP_LLTL_LIB_TYPE          := src
LSP_LLTL_LIB_URL           := https://github.com/sadko4u/$(LSP_LLTL_LIB_NAME).git

LSP_TEST_FW_VERSION        := 1.0.2
LSP_TEST_FW_NAME           := lsp-test-fw
LSP_TEST_FW_TYPE           := src
LSP_TEST_FW_URL            := https://github.com/sadko4u/$(LSP_TEST_FW_NAME).git

LIBSNDFILE_VERSION         := system
LIBSNDFILE_NAME            := sndfile
LIBSNDFILE_TYPE            := pkg

ifeq ($(PLATFORM),Windows)
  STDLIB_VERSION             := system
  STDLIB_TYPE                := opt
  STDLIB_LDFLAGS             := -lpthread -lshlwapi -lwinmm -lmsacm32
else ifeq ($(PLATFORM),BSD)
  STDLIB_VERSION             := system
  STDLIB_TYPE                := opt
  STDLIB_LDFLAGS             := -lpthread -ldl -liconv
else
  STDLIB_VERSION             := system
  STDLIB_TYPE                := opt
  STDLIB_LDFLAGS             := -lpthread -ldl
endif

ifeq ($(PLATFORM),Windows)
  TEST_STDLIB_VERSION        := system
  TEST_STDLIB_TYPE           := opt
  TEST_STDLIB_LDFLAGS        := 
else
  TEST_STDLIB_VERSION        := system
  TEST_STDLIB_TYPE           := opt
  TEST_STDLIB_LDFLAGS        := 
endif
