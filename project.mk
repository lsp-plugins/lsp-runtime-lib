#
# Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
#           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
#
# This file is part of lsp-runtime-lib
#
# lsp-runtime-lib is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
#
# lsp-runtime-lib is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with lsp-runtime-lib.  If not, see <https://www.gnu.org/licenses/>.
#

# Package version
ARTIFACT_ID                 = LSP_RUNTIME_LIB
ARTIFACT_NAME               = lsp-runtime-lib
ARTIFACT_DESC               = Runtime library used by LSP Project for plugin development
ARTIFACT_HEADERS            = lsp-plug.in
ARTIFACT_EXPORT_ALL         = 1
ARTIFACT_VERSION            = 0.5.10

# List of dependencies
DEPENDENCIES = \
  LIBPTHREAD \
  LIBDL \
  LSP_COMMON_LIB \
  LSP_LLTL_LIB

TEST_DEPENDENCIES = \
  LSP_TEST_FW

# Platform-specific dependencies
ifeq ($(PLATFORM),Linux)
  DEPENDENCIES             += \
    LIBSNDFILE
endif

ifeq ($(PLATFORM),BSD)
  DEPENDENCIES             += \
    LIBSNDFILE \
    LIBICONV
endif

ifeq ($(PLATFORM),Windows)
  DEPENDENCIES             += \
    LIBSHLWAPI \
    LIBWINMM \
    LIBMSACM
endif

ALL_DEPENDENCIES = \
  $(DEPENDENCIES) \
  $(TEST_DEPENDENCIES) \
  LIBSNDFILE \
  LIBICONV \
  LIBSHLWAPI \
  LIBWINMM \
  LIBMSACM

