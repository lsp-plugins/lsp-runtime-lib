/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 6 мар. 2019 г.
 *
 * lsp-runtime-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-runtime-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-runtime-lib. If not, see <https://www.gnu.org/licenses/>.
 */

#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/ByteBuffer.h>
#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/stdlib/string.h>
#include <lsp-plug.in/io/StdioFile.h>
#include <lsp-plug.in/io/NativeFile.h>
#include <lsp-plug.in/io/InFileStream.h>
#include <lsp-plug.in/io/InSequence.h>
#include <lsp-plug.in/io/OutFileStream.h>

using namespace lsp;
using namespace lsp::io;

UTEST_BEGIN("runtime.io", file)

    void testClosedFile(const char *label, File &fd)
    {
        uint8_t tmpbuf[0x100];

        printf("Testing %s...\n", label);

        // Test for read failues
        UTEST_ASSERT(fd.read(tmpbuf, sizeof(tmpbuf)) < 0);
        UTEST_ASSERT(fd.pread(0, tmpbuf, sizeof(tmpbuf)) < 0);

        // Test for write failures
        UTEST_ASSERT(fd.write(tmpbuf, sizeof(tmpbuf) < 0));
        UTEST_ASSERT(fd.pwrite(0, tmpbuf, sizeof(tmpbuf) < 0));
        UTEST_ASSERT(fd.position() < 0);

        // Test for supplementary failures
        UTEST_ASSERT(fd.seek(0, File::FSK_SET) != STATUS_OK);
        UTEST_ASSERT(fd.position() < 0);
        UTEST_ASSERT(fd.flush() != STATUS_OK);
        UTEST_ASSERT(fd.sync() != STATUS_OK);

        // Test for close success
        UTEST_ASSERT(fd.close() == STATUS_OK);
    }

    void testWriteonlyFile(File &fd)
    {
        uint32_t tmpbuf[0x100];

        UTEST_ASSERT(fd.seek(0, SEEK_END) == STATUS_OK);
        UTEST_ASSERT(fd.position() == 0);

        // Write data to file
        wssize_t written = 0;
        for (size_t i=0; i<0x100; ++i)
        {
            for (size_t j=0; j<0x100; ++j)
                tmpbuf[j] = (i << 8) | j;

            UTEST_ASSERT(fd.write(tmpbuf, sizeof(tmpbuf)) == sizeof(tmpbuf));
            written += sizeof(tmpbuf);
        }
        UTEST_ASSERT(fd.flush() == STATUS_OK);

        // Get status
        io::fattr_t attr;
        UTEST_ASSERT(fd.stat(&attr) == STATUS_OK);
        UTEST_ASSERT(attr.type == io::fattr_t::FT_REGULAR);
        UTEST_ASSERT(attr.size == wsize_t(written));

        // Obtain position and change it
        UTEST_ASSERT(fd.position() == written);
        wssize_t position = 0x1000;
        UTEST_ASSERT(fd.seek(position, SEEK_SET) == STATUS_OK);
        UTEST_ASSERT(fd.position() == position);

        // Ensure that read gives failures
        UTEST_ASSERT(fd.read(tmpbuf, sizeof(tmpbuf)) < 0);
        UTEST_ASSERT(fd.read(tmpbuf, 0) < 0);
        UTEST_ASSERT(fd.pread(0x10000, tmpbuf, sizeof(tmpbuf)) < 0);
        UTEST_ASSERT(fd.pread(0x10000, tmpbuf, 0) < 0);
        UTEST_ASSERT(fd.position() == position);

        // Ensure that positional write gives no failures
        ::memset(tmpbuf, 0x55, sizeof(tmpbuf));
        UTEST_ASSERT(fd.write(tmpbuf, sizeof(tmpbuf)) == sizeof(tmpbuf));
        UTEST_ASSERT(fd.write(tmpbuf, 0) == 0);
        position    += sizeof(tmpbuf);
        UTEST_ASSERT(fd.position() == position);
        ::memset(tmpbuf, 0xaa, sizeof(tmpbuf));
        UTEST_ASSERT(fd.pwrite(written, tmpbuf, sizeof(tmpbuf)) == sizeof(tmpbuf));
        UTEST_ASSERT(fd.pwrite(written, tmpbuf, 0) == 0);
        UTEST_ASSERT(fd.position() == position);

        // Ensure that sync() and flush() work properly
        UTEST_ASSERT(fd.flush() == STATUS_OK);
        UTEST_ASSERT(fd.sync() == STATUS_OK);

        // Try to truncate file
        UTEST_ASSERT(fd.truncate(written) == STATUS_OK);
        UTEST_ASSERT(fd.seek(0, SEEK_END) == STATUS_OK);
        UTEST_ASSERT(fd.position() == written);
        UTEST_ASSERT(fd.pwrite(written, tmpbuf, sizeof(tmpbuf)) == sizeof(tmpbuf));

        // Test for close success
        UTEST_ASSERT(fd.close() == STATUS_OK);
    }

    void testReadonlyFile(File &fd)
    {
        uint32_t tmpbuf[0x100], ckbuf[0x100];
        UTEST_ASSERT(fd.position() == 0);

        // Read data from file
        wssize_t read = 0;
        for (size_t i=0; i<0x100; ++i)
        {
            if (read != 0x1000)
            {
                for (size_t j=0; j<0x100; ++j)
                    ckbuf[j] = (i << 8) | j;
            }
            else
                ::memset(ckbuf, 0x55, sizeof(ckbuf));

            UTEST_ASSERT(fd.read(tmpbuf, sizeof(tmpbuf)) == sizeof(tmpbuf));
            UTEST_ASSERT(::memcmp(tmpbuf, ckbuf, sizeof(tmpbuf)) == 0);
            read += sizeof(tmpbuf);
        }

        // Read the last chunk and check EOF detection
        ::memset(ckbuf, 0xaa, sizeof(ckbuf));
        UTEST_ASSERT(fd.read(tmpbuf, sizeof(tmpbuf)) == sizeof(tmpbuf));
        UTEST_ASSERT(::memcmp(tmpbuf, ckbuf, sizeof(tmpbuf)) == 0);
        read += sizeof(tmpbuf);
        UTEST_ASSERT(fd.read(tmpbuf, sizeof(tmpbuf)) == (-STATUS_EOF));

        // Get status
        io::fattr_t attr;
        UTEST_ASSERT(fd.stat(&attr) == STATUS_OK);
        UTEST_ASSERT(attr.type == io::fattr_t::FT_REGULAR);
        UTEST_ASSERT(attr.size == wsize_t(read));

        // Obtain position and change it
        UTEST_ASSERT(fd.position() == read);
        wssize_t position = 0x1000;
        UTEST_ASSERT(fd.seek(position, SEEK_SET) == STATUS_OK);
        UTEST_ASSERT(fd.position() == position);

        // Ensure that write gives failures
        UTEST_ASSERT(fd.write(tmpbuf, sizeof(tmpbuf)) < 0);
        UTEST_ASSERT(fd.write(tmpbuf, 0) < 0);
        UTEST_ASSERT(fd.pwrite(0x10000, tmpbuf, sizeof(tmpbuf)) < 0);
        UTEST_ASSERT(fd.pwrite(0x10000, tmpbuf, 0) < 0);
        UTEST_ASSERT(fd.position() == position);

        // Ensure that positional read gives no failures
        ::memset(ckbuf, 0x55, sizeof(ckbuf));
        UTEST_ASSERT(fd.read(tmpbuf, sizeof(tmpbuf)) == sizeof(tmpbuf));
        UTEST_ASSERT(fd.read(tmpbuf, 0) == 0);
        UTEST_ASSERT(::memcmp(tmpbuf, ckbuf, sizeof(tmpbuf)) == 0);
        position    += sizeof(tmpbuf);
        UTEST_ASSERT(fd.position() == position);
        ::memset(ckbuf, 0xaa, sizeof(ckbuf));
        UTEST_ASSERT(fd.pread(read - sizeof(tmpbuf), tmpbuf, sizeof(tmpbuf)) == sizeof(tmpbuf));
        UTEST_ASSERT(fd.pread(read - sizeof(tmpbuf), tmpbuf, 0) == 0);
        UTEST_ASSERT(fd.position() == position);

        // Ensure that sync() and flush() do not work
        UTEST_ASSERT(fd.flush() != STATUS_OK);
        UTEST_ASSERT(fd.sync() != STATUS_OK);

        // Try to truncate file
        UTEST_ASSERT(fd.truncate(0x1000) != STATUS_OK);
        UTEST_ASSERT(fd.seek(0, SEEK_END) == STATUS_OK);
        UTEST_ASSERT(fd.position() == read);

        // Test for close success
        UTEST_ASSERT(fd.close() == STATUS_OK);
    }

    template <class TemplateFile>
        void testWriteonlyFileName(const char *label, const LSPString *path, TemplateFile &fd)
        {
            printf("Testing %s...\n", label);

            // Open file with creation and truncation
            UTEST_ASSERT(fd.open(path, File::FM_WRITE | File::FM_CREATE | File::FM_TRUNC) == STATUS_OK);
            testWriteonlyFile(fd);
        }

    template <class TemplateFile>
        void testReadonlyFileName(const char *label, const LSPString *path, TemplateFile &fd)
        {
            printf("Testing %s...\n", label);

            // Open file
            UTEST_ASSERT(fd.open(path, File::FM_READ) == STATUS_OK);
            testReadonlyFile(fd);
        }

    void testWriteonlyDescriptor(const char *label, FILE *f, StdioFile &fd)
    {
        printf("Testing %s...\n", label);
        UTEST_ASSERT(fd.wrap(f, File::FM_WRITE, false)  == STATUS_OK);
        testWriteonlyFile(fd);
    }

    void testReadonlyDescriptor(const char *label, FILE *f, StdioFile &fd)
    {
        printf("Testing %s...\n", label);
        UTEST_ASSERT(fd.wrap(f, File::FM_READ, false)  == STATUS_OK);
        testReadonlyFile(fd);
    }

    template <class TemplateFile>
        void testUnexistingFile(const char *label, TemplateFile &fd)
        {
            printf("Testing %s...\n", label);

            LSPString path;
            UTEST_ASSERT(path.fmt_utf8("%s" FILE_SEPARATOR_S "utest-nonexisting-%s.tmp", tempdir(), full_name()));

            UTEST_ASSERT(fd.open(&path, File::FM_WRITE) != STATUS_OK);
            UTEST_ASSERT(fd.close() == STATUS_OK);
        }

    void createFile(const io::Path *prefix, int num)
    {
        io::Path path;
        const char *buf;
        io::OutFileStream ofs;
        LSPString postfix;

        printf("  creating file: %d\n", num);

        // Form file name
        UTEST_ASSERT(postfix.fmt_ascii("rendel-%02d.tmp", num) > 0);
        UTEST_ASSERT(path.set(prefix) == STATUS_OK);
        UTEST_ASSERT(path.append(&postfix) == STATUS_OK);

        // Write data to file and close
        UTEST_ASSERT(ofs.open(&path, io::File::FM_WRITE_NEW) == STATUS_OK);
        UTEST_ASSERT((buf = path.as_utf8()) != NULL);
        UTEST_ASSERT(ofs.write(buf, strlen(buf)) > 0);
        UTEST_ASSERT(ofs.close() == STATUS_OK);
    }

    void renameFile(const io::Path *prefix, int from, int to, int mode)
    {
        io::Path f, t;
        LSPString postfix;
        printf("  renaming file: %d -> %d\n", from, to);

        // Form source file name
        UTEST_ASSERT(postfix.fmt_ascii("rendel-%02d.tmp", from) > 0);
        UTEST_ASSERT(f.set(prefix) == STATUS_OK);
        UTEST_ASSERT(f.append(&postfix) == STATUS_OK);

        // Form destination file name
        UTEST_ASSERT(postfix.fmt_ascii("rendel-%02d.tmp", to) > 0);
        UTEST_ASSERT(t.set(prefix) == STATUS_OK);
        UTEST_ASSERT(t.append(&postfix) == STATUS_OK);

        // Rename the file
        status_t res = STATUS_NOT_IMPLEMENTED;
        switch (mode)
        {
            case 0: res = io::File::rename(f.as_utf8(), t.as_utf8()); break;
            case 1: res = io::File::rename(f.as_utf8(), t.as_string()); break;
            case 2: res = io::File::rename(f.as_utf8(), &t); break;
            case 3: res = io::File::rename(f.as_string(), t.as_utf8()); break;
            case 4: res = io::File::rename(f.as_string(), t.as_string()); break;
            case 5: res = io::File::rename(f.as_string(), &t); break;
            case 6: res = io::File::rename(&f, t.as_utf8()); break;
            case 7: res = io::File::rename(&f, t.as_string()); break;
            case 8: res = io::File::rename(&f, &t); break;
            default: break;
        }

        UTEST_ASSERT(res == STATUS_OK);
    }

    void checkFile(const io::Path *prefix, int from, int to)
    {
        io::Path f, t;
        io::InFileStream ifs;
        io::InSequence is;
        LSPString postfix, payload;
        printf("  checking file: unexists %d, exists %d\n", from, to);

        // Form source file name
        UTEST_ASSERT(postfix.fmt_ascii("rendel-%02d.tmp", from) > 0);
        UTEST_ASSERT(f.set(prefix) == STATUS_OK);
        UTEST_ASSERT(f.append(&postfix) == STATUS_OK);
        UTEST_ASSERT(!f.exists());

        // Form destination file name
        UTEST_ASSERT(postfix.fmt_ascii("rendel-%02d.tmp", to) > 0);
        UTEST_ASSERT(t.set(prefix) == STATUS_OK);
        UTEST_ASSERT(t.append(&postfix) == STATUS_OK);

        // Open file
        UTEST_ASSERT(ifs.open(&t) == STATUS_OK);
        UTEST_ASSERT(is.wrap(&ifs, WRAP_NONE, "UTF-8") == STATUS_OK);
        UTEST_ASSERT(is.read_line(&payload, true) == STATUS_OK);
        UTEST_ASSERT(is.close() == STATUS_OK);
        UTEST_ASSERT(ifs.close() == STATUS_OK);

        // Test the output
        UTEST_ASSERT_MSG(payload.equals(f.as_string()),
                "Payload: %s, expected: %s", payload.get_native(), f.as_native()
        );
    }

    void testRenameDelete()
    {
        printf("Testing rename and delete...\n");

        io::Path prefix;
        UTEST_ASSERT(prefix.fmt("%s/utest-%s-rendel", tempdir(), full_name()));

        // Create files
        for (int i=0; i<18; ++i)
            createFile(&prefix, i);

        // Rename/move files
        for (int i=0; i<18; i += 2)
            renameFile(&prefix, i, i+1, i >> 1);

        // Check file
        for (int i=0; i<18; i += 2)
            checkFile(&prefix, i, i+1);
    }

    void testCopy()
    {
        printf("Testing file copy...\n");

        ssize_t res, nread;
        io::Path dst, src;
        io::NativeFile in, out;
        UTEST_ASSERT(src.fmt("%s/utest-%s-copy-src.bin", tempdir(), full_name()) > 0);
        UTEST_ASSERT(dst.fmt("%s/utest-%s-copy-dst.bin", tempdir(), full_name()) > 0);

        // Generate source file
        printf("  generating source file...\n");
        ByteBuffer buf1(0x1000), buf2(0x1000);
        UTEST_ASSERT(in.open(&src, io::File::FM_READWRITE_NEW) == STATUS_OK);
        wssize_t written = 0;

        for (size_t i=0; i<0x100; ++i)
        {
            buf1.randomize();
            UTEST_ASSERT(in.write(buf1.data(), buf1.size()) == ssize_t(buf1.size()));
            written    += buf1.size();
        }

        UTEST_ASSERT(in.flush() == STATUS_OK);

        // Copy the file
        printf("  copying file %s to %s...\n", src.as_native(), dst.as_native());
        wssize_t copied = io::File::copy(&src, &dst, 0x1234);
        UTEST_ASSERT_MSG(copied == written,
            "Expected to be written %ld bytes but written %ld bytes",
            long(written), long(copied));

        // Validate the file
        printf("  verifying file contents...\n");
        UTEST_ASSERT(in.seek(0, io::File::FSK_SET) == STATUS_OK);
        UTEST_ASSERT(out.open(&dst, io::File::FM_READ) == STATUS_OK);

        copied = 0;
        while ((res = in.read(buf1.data(), buf1.size())) >= 0)
        {
            nread = out.read(buf2.data(), res);
            UTEST_ASSERT(nread == res);
            UTEST_ASSERT(buf1.equals(buf2.data(), res));

            copied += nread;
        }
        UTEST_ASSERT(res == -STATUS_EOF);
        UTEST_ASSERT(in.close() == STATUS_OK);
        UTEST_ASSERT(out.close() == STATUS_OK);
        UTEST_ASSERT(copied == written);

        printf("  all is ok, %d bytes copied\n", written);
    }

    UTEST_MAIN
    {
        LSPString path;

        File none_fd;
        StdioFile std_fd;
        NativeFile native_fd;

        UTEST_ASSERT(path.fmt_utf8("%s" FILE_SEPARATOR_S "utest-%s.tmp", tempdir(), full_name()));

        // Test closed files, all should fail the same way
        testClosedFile("test_closed_file (abstract)", none_fd);
        testClosedFile("test_closed_file (stdio)", std_fd);
        testClosedFile("test_closed_file (native)", native_fd);

        // Test stdio file
        testWriteonlyFileName("test_writeonly_filename (stdio)", &path, std_fd);
        testReadonlyFileName("test_readonly_filename (stdio)", &path, std_fd);
        testUnexistingFile("test_unexsiting_file (stdio)", std_fd);

        // Test stdio file as a wrapper
        FILE *fd = fopen(path.get_native(), "wb+");
        UTEST_ASSERT(fd != NULL);
        testWriteonlyDescriptor("test_writeonly_descriptor (stdio)", fd, std_fd);
        UTEST_ASSERT(fseek(fd, 0, SEEK_SET) == 0);
        testReadonlyDescriptor("test_readonly_descriptor (stdio)", fd, std_fd);
        UTEST_ASSERT(fclose(fd) == 0);

        // Test native file
        testWriteonlyFileName("test_writeonly_filename (native)", &path, native_fd);
        testReadonlyFileName("test_readonly_filename (native)", &path, native_fd);
        testUnexistingFile("test_unexsiting_file (native)", native_fd);

        // Test rename and delete
        testRenameDelete();

        // Test file copy
        testCopy();
    }

UTEST_END

