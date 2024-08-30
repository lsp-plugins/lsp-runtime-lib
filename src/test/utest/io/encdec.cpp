/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 9 мар. 2019 г.
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
#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/stdlib/locale.h>
#include <lsp-plug.in/runtime/LSPString.h>
#include <lsp-plug.in/io/NativeFile.h>
#include <lsp-plug.in/io/CharsetEncoder.h>
#include <lsp-plug.in/io/CharsetDecoder.h>

#include <string.h>

using namespace lsp;
using namespace lsp::io;

// Test buffer size is a simple number, more than 0x1000
#define BUFFER_SIZE         4567

UTEST_BEGIN("runtime.io", encdec)

    void testDecodeFile(const LSPString *src, const LSPString *dst, const char *charset)
    {
        NativeFile in, out;
        CharsetDecoder decoder;

        printf("  decoding %s (%s) -> %s (UTF-32)\n", src->get_native(), charset, dst->get_native());

        UTEST_ASSERT(in.open(src, File::FM_READ) == STATUS_OK);
        UTEST_ASSERT(out.open(dst, File::FM_WRITE | File::FM_CREATE | File::FM_TRUNC) == STATUS_OK);
        UTEST_ASSERT(decoder.init(charset) == STATUS_OK);

        lsp_wchar_t *obuf = new lsp_wchar_t[BUFFER_SIZE];
        UTEST_ASSERT(obuf != NULL);

        ssize_t fetched, filled;

        do
        {
            // Is there any data on output?
            fetched = decoder.fetch(obuf, BUFFER_SIZE);
            if (fetched > 0)
            {
                ssize_t to_write = fetched * sizeof(lsp_wchar_t);
                ssize_t written = out.write(obuf, to_write);
                UTEST_ASSERT(written == to_write);
            }
            else if (fetched < 0)
                UTEST_FAIL_MSG("decoder.fetch() failed with error %d", int(-fetched));

            // Is there any data on input?
            filled = decoder.fill(&in);
            if ((filled < 0) && (filled != -STATUS_EOF))
                UTEST_FAIL_MSG("decoder.fill() failed with error %d", int(-filled));

        } while ((fetched > 0) || (filled > 0));

        decoder.close();
        UTEST_ASSERT(out.flush() == STATUS_OK);
        UTEST_ASSERT(out.size() > 0);
        UTEST_ASSERT(out.close() == STATUS_OK);
        UTEST_ASSERT(in.close() == STATUS_OK);

        delete [] obuf;
    }

    void testEncodeFile(const LSPString *src, const LSPString *dst, const char *charset)
    {
        NativeFile in, out;
        CharsetEncoder encoder;

        printf("  encoding %s (UTF-32) -> %s (%s)\n", src->get_native(), dst->get_native(), charset);

        UTEST_ASSERT(in.open(src, File::FM_READ) == STATUS_OK);
        UTEST_ASSERT(out.open(dst, File::FM_WRITE | File::FM_CREATE | File::FM_TRUNC) == STATUS_OK);
        UTEST_ASSERT(encoder.init(charset) == STATUS_OK);

        lsp_wchar_t *ibuf = new lsp_wchar_t[BUFFER_SIZE];
        UTEST_ASSERT(ibuf != NULL);
        lsp_wchar_t *ibufh = ibuf, *ibuft = ibuf;

        ssize_t fetched, filled;

        do
        {
            // Is there any data on output?
            fetched = encoder.fetch(&out, BUFFER_SIZE);
            if ((fetched < 0) && (fetched != -STATUS_EOF))
                UTEST_FAIL_MSG("decoder.fetch() failed with error %d", int(-fetched));

            // Do we need to fill the buffer with new data?
            size_t count = ibuft - ibufh;
            if (count < (BUFFER_SIZE >> 1))
            {
                if (count > 0)
                    ::memmove(ibuf, ibufh, count * sizeof(lsp_wchar_t));
                ibufh   = ibuf;
                ibuft   = &ibuf[count];

                // Try to perform additional read
                size_t to_read = BUFFER_SIZE - count;
                filled = in.read(ibuft, to_read * sizeof(lsp_wchar_t));
                if (filled > 0)
                {
                    UTEST_ASSERT((filled % sizeof(lsp_wchar_t)) == 0);
                    count  += filled / sizeof(lsp_wchar_t) ;
                    ibuft   = &ibufh[count];
                }
            }

            // Is there any data on input?
            filled  = encoder.fill(ibufh, count);
            if (filled > 0)
                ibufh      += filled;
            else if ((filled < 0) && (filled != -STATUS_EOF))
                UTEST_FAIL_MSG("decoder.fill() failed with error %d", int(-filled));

        } while ((fetched > 0) || (filled > 0));

        encoder.close();
        UTEST_ASSERT(out.flush() == STATUS_OK);
        UTEST_ASSERT(out.size() > 0);
        UTEST_ASSERT(out.close() == STATUS_OK);
        UTEST_ASSERT(in.close() == STATUS_OK);

        delete [] ibuf;
    }
    
    void compareFiles(const LSPString *src, const LSPString *dst)
    {
        NativeFile f1, f2;
        uint8_t *b1, *b2;

        printf("  comparing %s <-> %s\n", src->get_native(), dst->get_native());

        UTEST_ASSERT(f1.open(src, File::FM_READ) == STATUS_OK);
        UTEST_ASSERT(f2.open(dst, File::FM_READ) == STATUS_OK);
        UTEST_ASSERT(f1.size() == f2.size());

        b1 = new uint8_t[0x1000];
        UTEST_ASSERT(b1 != NULL);
        b2 = new uint8_t[0x1000];
        UTEST_ASSERT(b2 != NULL);

        while (true)
        {
            wssize_t offset = f1.position();
            ssize_t read1 = f1.read(b1, 0x1000);
            ssize_t read2 = f2.read(b2, 0x1000);

            // Check read status
            UTEST_ASSERT(read1 == read2);
            if ((read1 == 0) || (read2 == (-STATUS_EOF)))
                break;
            else if (read1 < 0)
                UTEST_FAIL_MSG("read returned %d", int(read1));

            // Perform check
            UTEST_ASSERT_MSG((::memcmp(b1, b2, read1) == 0), "Blocks offset=0x%x, size=0x%x differ", int(offset), int(read1));
        }

        UTEST_ASSERT(f1.close() == STATUS_OK);
        UTEST_ASSERT(f2.close() == STATUS_OK);

        delete [] b1;
        delete [] b2;
    }
    
    void testFileCoding(const char *base, const char *src, const char *charset)
    {
        LSPString fenc, fdec, fsrc;
        UTEST_ASSERT(fenc.fmt_utf8("%s" FILE_SEPARATOR_S "utest-%s-encoded.tmp", tempdir(), full_name()));
        UTEST_ASSERT(fdec.fmt_utf8("%s" FILE_SEPARATOR_S "utest-%s-decoded.tmp", tempdir(), full_name()));

        UTEST_ASSERT(fsrc.fmt_utf8("%s" FILE_SEPARATOR_S "%s" FILE_SEPARATOR_S "%s", resources(), base, src));
        printf("Testing encoders on file %s...\n", fsrc.get_native());
        testDecodeFile(&fsrc, &fenc, charset);
        testEncodeFile(&fenc, &fdec, charset);
        compareFiles(&fsrc, &fdec);
    }

    UTEST_MAIN
    {
        const char *base = "io" FILE_SEPARATOR_S "iconv";

        testFileCoding(base, "01-de-utf16le.txt", "UTF-16LE");
        testFileCoding(base, "01-de-utf8.txt", "UTF-8");
        testFileCoding(base, "02-ja-utf16le.txt", "UTF-16LE");
        testFileCoding(base, "02-ja-utf8.txt", "UTF-8");
        testFileCoding(base, "03-ru-cp1251.txt", "CP1251");
        testFileCoding(base, "03-ru-utf16le.txt", "UTF-16LE");
        testFileCoding(base, "03-ru-utf8.txt", "UTF-8");
    }

UTEST_END

