# Package version
ARTIFACT_NAME               = lsp-runtime-lib
ARTIFACT_VARS               = LSP_RUNTIME_LIB
ARTIFACT_HEADERS            = lsp-plug.in
ARTIFACT_EXPORT_ALL         = 1
ARTIFACT_VERSION            = 0.5.4

# List of dependencies
TEST_DEPENDENCIES = \
  TEST_STDLIB \
  LSP_TEST_FW

DEPENDENCIES = \
  STDLIB \
  LSP_COMMON_LIB \
  LSP_LLTL_LIB

LINUX_DEPENDENCIES = \
  LIBSNDFILE

BSD_DEPENDENCIES = \
  LIBSNDFILE

# For Linux-based systems, use libsndfile
ifeq ($(PLATFORM),Linux)
  DEPENDENCIES             += $(LINUX_DEPENDENCIES)
endif

# For BSD-based systems, use libsndfile
ifeq ($(PLATFORM),BSD)
  DEPENDENCIES             += $(BSD_DEPENDENCIES)
endif

ALL_DEPENDENCIES = \
  $(DEPENDENCIES) \
  $(TEST_DEPENDENCIES) \
  $(LINUX_DEPENDENCIES) \
  $(BSD_DEPENDENCIES)