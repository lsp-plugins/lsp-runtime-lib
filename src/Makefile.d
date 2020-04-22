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
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/alloc.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/charset.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h
$(LSP_RUNTIME_LIB_BIN)/main/protocol/osc/pattern.o: \
 main/protocol/osc/pattern.cpp \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/alloc.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/midi.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc/forge.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc/parse.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc/pattern.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc/debug.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/debug.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h
$(LSP_RUNTIME_LIB_BIN)/main/protocol/osc/parse.o: \
 main/protocol/osc/parse.cpp \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/endian.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/arch/x86/endian.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/debug.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/midi.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc/forge.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc/parse.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc/pattern.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc/debug.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/math.h
$(LSP_RUNTIME_LIB_BIN)/main/protocol/osc/forge.o: \
 main/protocol/osc/forge.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/midi.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc/forge.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc/parse.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc/pattern.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc/debug.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/debug.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/endian.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/arch/x86/endian.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/math.h
$(LSP_RUNTIME_LIB_BIN)/main/protocol/osc/debug.o: \
 main/protocol/osc/debug.cpp \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/endian.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/arch/x86/endian.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/debug.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/midi.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc/forge.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc/parse.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc/pattern.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc/debug.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h
$(LSP_RUNTIME_LIB_BIN)/main/protocol/midi.o: main/protocol/midi.cpp \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/midi.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h
$(LSP_RUNTIME_LIB_BIN)/main/mm/types.o: main/mm/types.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/mm/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h
$(LSP_RUNTIME_LIB_BIN)/main/mm/sample.o: main/mm/sample.cpp \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/endian.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/arch/x86/endian.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/mm/sample.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/mm/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h
$(LSP_RUNTIME_LIB_BIN)/main/mm/OutAudioFileStream.o: \
 main/mm/OutAudioFileStream.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/mm/OutAudioFileStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/mm/IOutAudioStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/mm/types.h
$(LSP_RUNTIME_LIB_BIN)/main/mm/IOutAudioStream.o: \
 main/mm/IOutAudioStream.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/mm/IOutAudioStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/mm/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/mm/sample.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/alloc.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h
$(LSP_RUNTIME_LIB_BIN)/main/mm/InAudioFileStream.o: \
 main/mm/InAudioFileStream.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/mm/InAudioFileStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/mm/IInAudioStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/mm/types.h
$(LSP_RUNTIME_LIB_BIN)/main/mm/IInAudioStream.o: \
 main/mm/IInAudioStream.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/mm/IInAudioStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/mm/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/mm/sample.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/alloc.h
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
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/endian.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/arch/x86/endian.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/charset.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/xml/PushParser.o: \
 main/fmt/xml/PushParser.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/xml/PushParser.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/xml/PullParser.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInStream.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/xml/const.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/parray.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/xml/IXMLHandler.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/xml/PullParser.o: \
 main/fmt/xml/PullParser.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InStringSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/charset.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/File.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/CharsetDecoder.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InFileStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/xml/PullParser.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/xml/const.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/parray.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/version.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/xml/IXMLHandler.o: \
 main/fmt/xml/IXMLHandler.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/xml/IXMLHandler.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/xml/const.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/xml/const.o: main/fmt/xml/const.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/xml/const.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/url.o: main/fmt/url.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/url.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/RoomEQWizard.o: main/fmt/RoomEQWizard.cpp \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/alloc.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/endian.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/arch/x86/endian.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/debug.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/math.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InFileStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/File.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/OutMemoryStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InMemoryStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/charset.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/CharsetDecoder.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/RoomEQWizard.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/ObjectStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/const.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/Object.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/String.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/RawArray.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/ObjectStreamField.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/Enum.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/Handles.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/ObjectStreamClass.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/darray.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/version.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/obj/PullParser.o: \
 main/fmt/obj/PullParser.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/obj/PullParser.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/obj/const.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/darray.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InFileStream.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/File.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InStringSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/charset.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/CharsetDecoder.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutSequence.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/obj/IObjHandler.o: \
 main/fmt/obj/IObjHandler.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/obj/IObjHandler.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/obj/const.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/darray.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/version.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/lspc/File.o: main/fmt/lspc/File.cpp \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/endian.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/arch/x86/endian.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/lspc.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/File.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/ChunkReader.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/ChunkAccessor.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/ChunkWriter.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/lspc/ChunkWriter.o: \
 main/fmt/lspc/ChunkWriter.cpp \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/endian.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/arch/x86/endian.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/ChunkWriter.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/ChunkAccessor.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/lspc.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/lspc/ChunkReader.o: \
 main/fmt/lspc/ChunkReader.cpp \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/endian.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/arch/x86/endian.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/lspc.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/ChunkReader.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/ChunkAccessor.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/lspc/ChunkAccessor.o: \
 main/fmt/lspc/ChunkAccessor.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/ChunkAccessor.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/debug.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/lspc/AudioWriter.o: \
 main/fmt/lspc/AudioWriter.cpp \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/endian.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/arch/x86/endian.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/AudioWriter.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/File.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/ChunkReader.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/ChunkAccessor.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/ChunkWriter.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/lspc.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/lspc/AudioReader.o: \
 main/fmt/lspc/AudioReader.cpp \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/endian.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/arch/x86/endian.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/AudioReader.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/File.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/ChunkReader.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/ChunkAccessor.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/ChunkWriter.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/lspc.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/json/Tokenizer.o: \
 main/fmt/json/Tokenizer.cpp \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/math.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/json/Tokenizer.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/json/token.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/json/Serializer.o: \
 main/fmt/json/Serializer.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/File.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/OutFileStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/OutStringSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/OutSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/charset.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/CharsetEncoder.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/json/Tokenizer.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/json/token.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/json/Serializer.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/darray.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/math.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/json/Parser.o: main/fmt/json/Parser.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InStringSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/charset.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/File.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/CharsetDecoder.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InFileStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/json/Parser.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/json/token.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/json/Tokenizer.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/darray.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/version.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/java/wrappers.o: \
 main/fmt/java/wrappers.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/wrappers.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/const.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/Object.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/java/String.o: main/fmt/java/String.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/defs.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/const.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/String.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/Object.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/java/RawArray.o: \
 main/fmt/java/RawArray.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/defs.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/const.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/RawArray.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/Object.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/ObjectStreamField.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/String.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/java/ObjectStreamField.o: \
 main/fmt/java/ObjectStreamField.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/Object.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/const.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/ObjectStreamField.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/String.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/java/ObjectStream.o: \
 main/fmt/java/ObjectStream.cpp \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/endian.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/arch/x86/endian.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/alloc.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InFileStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/File.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InMemoryStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/defs.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/const.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/wrappers.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/Object.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/ObjectStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/String.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/RawArray.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/ObjectStreamField.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/Enum.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/Handles.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/ObjectStreamClass.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/java/ObjectStreamClass.o: \
 main/fmt/java/ObjectStreamClass.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/defs.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/const.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/ObjectStreamClass.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/Object.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/ObjectStreamField.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/String.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/java/Object.o: main/fmt/java/Object.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/defs.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/const.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/Object.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/String.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/RawArray.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/ObjectStreamField.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/Enum.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/wrappers.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/ObjectStreamClass.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/java/Handles.o: main/fmt/java/Handles.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/Handles.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/Object.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/const.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/java/Enum.o: main/fmt/java/Enum.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/Enum.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/Object.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/const.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/java/defs.o: main/fmt/java/defs.cpp \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/defs.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/const.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/Object.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/java/const.o: main/fmt/java/const.cpp \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/const.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/Object.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h
$(LSP_RUNTIME_LIB_BIN)/main/fmt/bookmarks.o: main/fmt/bookmarks.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/url.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/bookmarks.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/parray.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/json/Parser.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/json/token.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/json/Tokenizer.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/darray.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/json/Serializer.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/xml/PushParser.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/xml/PullParser.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/xml/const.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/xml/IXMLHandler.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InFileStream.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/File.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/charset.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/CharsetDecoder.h
$(LSP_RUNTIME_LIB_BIN)/main/expr/Variables.o: main/expr/Variables.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/Variables.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/Resolver.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/parray.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/version.h
$(LSP_RUNTIME_LIB_BIN)/main/expr/types.o: main/expr/types.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InStringSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/Tokenizer.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/token.h
$(LSP_RUNTIME_LIB_BIN)/main/expr/Tokenizer.o: main/expr/Tokenizer.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/Tokenizer.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/token.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/charset.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/math.h
$(LSP_RUNTIME_LIB_BIN)/main/expr/Resolver.o: main/expr/Resolver.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/Resolver.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h
$(LSP_RUNTIME_LIB_BIN)/main/expr/parser.o: main/expr/parser.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/parser.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/evaluator.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/Resolver.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/Tokenizer.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/token.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/parray.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/math.h
$(LSP_RUNTIME_LIB_BIN)/main/expr/Parameters.o: main/expr/Parameters.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/Parameters.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/parray.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/Resolver.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/alloc.h
$(LSP_RUNTIME_LIB_BIN)/main/expr/format.o: main/expr/format.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/format.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/Parameters.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/parray.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/Resolver.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/Tokenizer.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/token.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InStringSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/OutStringSequence.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/math.h
$(LSP_RUNTIME_LIB_BIN)/main/expr/Expression.o: main/expr/Expression.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InStringSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/parser.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/evaluator.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/Resolver.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/Tokenizer.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/token.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/Expression.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/darray.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/version.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/parray.h
$(LSP_RUNTIME_LIB_BIN)/main/expr/evaluator.o: main/expr/evaluator.cpp \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/parser.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/evaluator.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/Resolver.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/Tokenizer.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/token.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/math.h
$(LSP_RUNTIME_LIB_BIN)/test/protocol/osc.o: test/protocol/osc.cpp \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/endian.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/arch/x86/endian.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/helpers.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/midi.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc/forge.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc/parse.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc/pattern.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/osc/debug.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/debug.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/math.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h
$(LSP_RUNTIME_LIB_BIN)/test/protocol/midi.o: test/protocol/midi.cpp \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/helpers.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/ByteBuffer.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/protocol/midi.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h
$(LSP_RUNTIME_LIB_BIN)/test/mm/sample.o: test/mm/sample.cpp \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/endian.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/arch/x86/endian.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/ByteBuffer.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/mm/sample.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/mm/types.h
$(LSP_RUNTIME_LIB_BIN)/test/mm/outaudiofilestream.o: \
 test/mm/outaudiofilestream.cpp \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/ByteBuffer.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/FloatBuffer.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/helpers.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/mm/InAudioFileStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/mm/IInAudioStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/mm/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/mm/OutAudioFileStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/mm/IOutAudioStream.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/math.h
$(LSP_RUNTIME_LIB_BIN)/test/mm/inaudiofilestream.o: \
 test/mm/inaudiofilestream.cpp \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/ByteBuffer.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/FloatBuffer.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/helpers.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/mm/InAudioFileStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/mm/IInAudioStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/mm/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/math.h
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
$(LSP_RUNTIME_LIB_BIN)/test/fmt/xml/pushparser.o: \
 test/fmt/xml/pushparser.cpp \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/helpers.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/xml/PushParser.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/xml/PullParser.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/xml/const.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/parray.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/xml/IXMLHandler.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/math.h
$(LSP_RUNTIME_LIB_BIN)/test/fmt/xml/pullparser.o: \
 test/fmt/xml/pullparser.cpp \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/helpers.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/xml/PullParser.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/xml/const.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/parray.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/math.h
$(LSP_RUNTIME_LIB_BIN)/test/fmt/roomeqwizard.o: test/fmt/roomeqwizard.cpp \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/helpers.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/RoomEQWizard.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/File.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInStream.h
$(LSP_RUNTIME_LIB_BIN)/test/fmt/obj/pullparser.o: \
 test/fmt/obj/pullparser.cpp \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/helpers.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/obj/PullParser.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/obj/const.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/darray.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/version.h
$(LSP_RUNTIME_LIB_BIN)/test/fmt/lspc/lspc.o: test/fmt/lspc/lspc.cpp \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/endian.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/arch/x86/endian.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/ByteBuffer.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/File.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/ChunkReader.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/ChunkAccessor.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/ChunkWriter.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/lspc.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h
$(LSP_RUNTIME_LIB_BIN)/test/fmt/lspc/audio.o: test/fmt/lspc/audio.cpp \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/FloatBuffer.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/helpers.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/lspc.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/AudioWriter.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/File.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/ChunkReader.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/ChunkAccessor.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/stdio.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/ChunkWriter.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/lspc/AudioReader.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/parray.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/version.h
$(LSP_RUNTIME_LIB_BIN)/test/fmt/json/tokenizer.o: \
 test/fmt/json/tokenizer.cpp \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/helpers.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InStringSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/json/token.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/json/Tokenizer.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/math.h
$(LSP_RUNTIME_LIB_BIN)/test/fmt/json/serializer.o: \
 test/fmt/json/serializer.cpp \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/helpers.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/math.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/json/Serializer.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/json/token.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/darray.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/version.h
$(LSP_RUNTIME_LIB_BIN)/test/fmt/json/parser.o: test/fmt/json/parser.cpp \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/helpers.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/json/Parser.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/json/token.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/json/Tokenizer.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/darray.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/version.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/math.h
$(LSP_RUNTIME_LIB_BIN)/test/fmt/java.o: test/fmt/java.cpp \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/const.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/Handles.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/Object.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/ObjectStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInStream.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/String.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/RawArray.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/ObjectStreamField.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/Enum.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/java/ObjectStreamClass.h
$(LSP_RUNTIME_LIB_BIN)/test/fmt/bookmarks.o: test/fmt/bookmarks.cpp \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/fmt/bookmarks.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/parray.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/Path.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutSequence.h
$(LSP_RUNTIME_LIB_BIN)/test/expr/tokenizer.o: test/expr/tokenizer.cpp \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/helpers.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/token.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/Tokenizer.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/InStringSequence.h
$(LSP_RUNTIME_LIB_BIN)/test/expr/parameters.o: test/expr/parameters.cpp \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/helpers.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/Parameters.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/parray.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/Resolver.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h
$(LSP_RUNTIME_LIB_BIN)/test/expr/format.o: test/expr/format.cpp \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/helpers.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/format.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IOutSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/Parameters.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/parray.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/Resolver.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/types.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/math.h
$(LSP_RUNTIME_LIB_BIN)/test/expr/expression.o: test/expr/expression.cpp \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/utest.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/status.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/common/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/test.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/env.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/config.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/types.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/main/dynarray.h \
 $(LSP_TEST_FW_INC)/lsp-plug.in/test-fw/helpers.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/Expression.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/version.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/runtime/LSPString.h \
 $(LSP_COMMON_LIB_INC)/lsp-plug.in/stdlib/string.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/io/IInSequence.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/types.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/Resolver.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/darray.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/version.h \
 $(LSP_LLTL_LIB_INC)/lsp-plug.in/lltl/parray.h \
 $(LSP_RUNTIME_LIB_INC)/lsp-plug.in/expr/Variables.h
