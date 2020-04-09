$(LSP_RUNTIME_LIB_BIN)/main/runtime/system.o: main/runtime/system.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/system.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h
$(LSP_RUNTIME_LIB_BIN)/main/runtime/LSPString.o: \
 main/runtime/LSPString.cpp \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/alloc.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/charset.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h
$(LSP_RUNTIME_LIB_BIN)/main/ipc/Thread.o: main/ipc/Thread.cpp \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/atomic.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/arch/x86/atomic.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/Thread.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/IRunnable.h
$(LSP_RUNTIME_LIB_BIN)/main/ipc/Process.o: main/ipc/Process.cpp \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/debug.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/Process.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutStream.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/parray.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/OutFileStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/File.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InFileStream.h
$(LSP_RUNTIME_LIB_BIN)/main/ipc/NativeExecutor.o: \
 main/ipc/NativeExecutor.cpp \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/debug.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/NativeExecutor.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/atomic.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/arch/x86/atomic.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/Thread.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/IRunnable.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/IExecutor.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/ITask.h
$(LSP_RUNTIME_LIB_BIN)/main/ipc/Mutex.o: main/ipc/Mutex.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/Mutex.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/atomic.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/arch/x86/atomic.h
$(LSP_RUNTIME_LIB_BIN)/main/ipc/Library.o: main/ipc/Library.cpp \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/debug.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/Library.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h
$(LSP_RUNTIME_LIB_BIN)/main/ipc/ITask.o: main/ipc/ITask.cpp \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/ITask.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/IRunnable.h
$(LSP_RUNTIME_LIB_BIN)/main/ipc/IRunnable.o: main/ipc/IRunnable.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/IRunnable.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h
$(LSP_RUNTIME_LIB_BIN)/main/ipc/IExecutor.o: main/ipc/IExecutor.cpp \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/IExecutor.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/ITask.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/IRunnable.h
$(LSP_RUNTIME_LIB_BIN)/main/io/StdioFile.o: main/io/StdioFile.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/StdioFile.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/File.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h
$(LSP_RUNTIME_LIB_BIN)/main/io/Path.o: main/io/Path.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/File.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Dir.h
$(LSP_RUNTIME_LIB_BIN)/main/io/OutStringSequence.o: \
 main/io/OutStringSequence.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/OutStringSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h
$(LSP_RUNTIME_LIB_BIN)/main/io/OutSequence.o: main/io/OutSequence.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/charset.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/StdioFile.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/File.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/NativeFile.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/OutFileStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/OutSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/CharsetEncoder.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutSequence.h
$(LSP_RUNTIME_LIB_BIN)/main/io/OutMemoryStream.o: \
 main/io/OutMemoryStream.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/OutMemoryStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutStream.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h
$(LSP_RUNTIME_LIB_BIN)/main/io/OutFileStream.o: main/io/OutFileStream.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/StdioFile.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/File.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/NativeFile.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/OutFileStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutStream.h
$(LSP_RUNTIME_LIB_BIN)/main/io/NativeFile.o: main/io/NativeFile.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/NativeFile.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/File.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h
$(LSP_RUNTIME_LIB_BIN)/main/io/IOutStream.o: main/io/IOutStream.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h
$(LSP_RUNTIME_LIB_BIN)/main/io/IOutSequence.o: main/io/IOutSequence.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h
$(LSP_RUNTIME_LIB_BIN)/main/io/InStringSequence.o: \
 main/io/InStringSequence.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InStringSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h
$(LSP_RUNTIME_LIB_BIN)/main/io/InSequence.o: main/io/InSequence.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/charset.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/StdioFile.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/File.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/NativeFile.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InFileStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/CharsetDecoder.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h
$(LSP_RUNTIME_LIB_BIN)/main/io/InMemoryStream.o: \
 main/io/InMemoryStream.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InMemoryStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInStream.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h
$(LSP_RUNTIME_LIB_BIN)/main/io/InFileStream.o: main/io/InFileStream.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/NativeFile.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/File.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/StdioFile.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InFileStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInStream.h
$(LSP_RUNTIME_LIB_BIN)/main/io/IInStream.o: main/io/IInStream.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutStream.h
$(LSP_RUNTIME_LIB_BIN)/main/io/IInSequence.o: main/io/IInSequence.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h
$(LSP_RUNTIME_LIB_BIN)/main/io/File.o: main/io/File.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/File.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/debug.h
$(LSP_RUNTIME_LIB_BIN)/main/io/Dir.o: main/io/Dir.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Dir.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/File.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h
$(LSP_RUNTIME_LIB_BIN)/main/io/CharsetEncoder.o: \
 main/io/CharsetEncoder.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/CharsetEncoder.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/charset.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/File.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutStream.h
$(LSP_RUNTIME_LIB_BIN)/main/io/CharsetDecoder.o: \
 main/io/CharsetDecoder.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/CharsetDecoder.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/charset.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/File.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutSequence.h
$(LSP_RUNTIME_LIB_BIN)/main/io/charset.o: main/io/charset.cpp \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/endian.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/arch/x86/endian.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/charset.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h
$(LSP_RUNTIME_LIB_BIN)/test/main.o: test/main.cpp \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h
$(LSP_RUNTIME_LIB_BIN)/test/ipc/thread.o: test/ipc/thread.cpp \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/Thread.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/IRunnable.h
$(LSP_RUNTIME_LIB_BIN)/test/ipc/process.o: test/ipc/process.cpp \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/Process.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutStream.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/parray.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/Thread.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/IRunnable.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/system.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h
$(LSP_RUNTIME_LIB_BIN)/test/ipc/mutex.o: test/ipc/mutex.cpp \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/Thread.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/IRunnable.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/Mutex.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/atomic.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/arch/x86/atomic.h
$(LSP_RUNTIME_LIB_BIN)/test/ipc/executor.o: test/ipc/executor.cpp \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/Thread.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/IRunnable.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/NativeExecutor.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/atomic.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/arch/x86/atomic.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/IExecutor.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/ipc/ITask.h
$(LSP_RUNTIME_LIB_BIN)/test/io/sequence.o: test/io/sequence.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/charset.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/File.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/CharsetDecoder.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/NativeFile.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/OutSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/CharsetEncoder.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutStream.h
$(LSP_RUNTIME_LIB_BIN)/test/io/path.o: test/io/path.cpp \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h
$(LSP_RUNTIME_LIB_BIN)/test/io/file.o: test/io/file.cpp \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/StdioFile.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/File.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/NativeFile.h
$(LSP_RUNTIME_LIB_BIN)/test/io/encdec.o: test/io/encdec.cpp \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/NativeFile.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/File.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/CharsetEncoder.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/charset.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/CharsetDecoder.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutSequence.h
$(LSP_RUNTIME_LIB_BIN)/test/io/dir.o: test/io/dir.cpp \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Dir.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/File.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h
$(LSP_RUNTIME_LIB_BIN)/test/io/charset.o: test/io/charset.cpp \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/endian.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/arch/x86/endian.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/charset.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h
