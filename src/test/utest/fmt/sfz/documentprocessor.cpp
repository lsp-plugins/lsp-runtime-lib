/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 24 февр. 2023 г.
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

#include <lsp-plug.in/fmt/sfz/DocumentProcessor.h>
#include <lsp-plug.in/lltl/darray.h>
#include <lsp-plug.in/test-fw/utest.h>

UTEST_BEGIN("runtime.fmt.sfz", documentprocessor)

    class SFZHandler: public sfz::IDocumentHandler
    {
        protected:
            typedef struct pair_t
            {
                const char *key;
                const char *value;
            } pair_t;

            static ssize_t pairs_cmp(const pair_t *a, const pair_t *b)
            {
                return strcmp(a->key, b->key);
            }

        private:
            LSPString       sData;
            lltl::darray<pair_t> vFiles;

        protected:
            void dump_opcodes(const char **opcodes, const char **values)
            {
                lltl::darray<pair_t> pairs;
                for (;*opcodes != NULL; ++opcodes, ++values)
                {
                    pair_t *p = pairs.add();
                    p->key = *opcodes;
                    p->value = *values;
                }
                pairs.qsort(pairs_cmp);

                for (size_t i=0, n=pairs.size(); i<n; ++i)
                {
                    pair_t *p = pairs.uget(i);
                    if (i != 0)
                        sData.append(' ');
                    sData.fmt_append_ascii("%s=%s", p->key, p->value);
                }
            }

            static uint32_t hash_data(io::IInStream *data)
            {
                uint32_t hash = 0;
                ssize_t value;
                while ((value = data->read_byte()) >= 0)
                {
                    hash = (hash << 7) | (hash >> (sizeof(hash)*8 - 7));
                    hash += (value & 0xff) * 17;
                }
                return hash;
            }

        public:
            const LSPString *data() const { return &sData; }
            void add_file(const char *name, const char *data)
            {
                pair_t *p = vFiles.add();
                p->key = name;
                p->value = data;
            }

        public:
            virtual status_t begin() override
            {
                sData.append_ascii("// begin\n");
                return STATUS_OK;
            }

            virtual status_t control(const char **opcodes, const char **values)
            {
                sData.append_ascii("<control>\n");
                dump_opcodes(opcodes, values);
                sData.append('\n');
                return STATUS_OK;
            }

            virtual status_t region(const char **opcodes, const char **values)
            {
                sData.append_ascii("<region>\n");
                dump_opcodes(opcodes, values);
                sData.append('\n');
                return STATUS_OK;
            }

            virtual status_t sample(
                const char *name, io::IInStream *data,
                const char **opcodes, const char **values)
            {
                sData.append_ascii("<sample>\n");
                sData.fmt_append_ascii("name=%s data=0x%08x", name, int(hash_data(data)));
                if (*opcodes != NULL)
                    sData.append(' ');
                dump_opcodes(opcodes, values);
                sData.append('\n');
                return STATUS_OK;
            }

            virtual status_t effect(const char **opcodes, const char **values)
            {
                sData.append_ascii("<effect>\n");
                dump_opcodes(opcodes, values);
                sData.append('\n');
                return STATUS_OK;
            }

            virtual status_t midi(const char **opcodes, const char **values)
            {
                sData.append_ascii("<midi>\n");
                dump_opcodes(opcodes, values);
                sData.append('\n');
                return STATUS_OK;
            }

            virtual status_t curve(const char **opcodes, const char **values)
            {
                sData.append_ascii("<curve>\n");
                dump_opcodes(opcodes, values);
                sData.append('\n');
                return STATUS_OK;
            }

            virtual status_t custom_header(const char *name, const char **opcodes, const char **values)
            {
                sData.fmt_append_ascii("<%s>\n", name);
                dump_opcodes(opcodes, values);
                sData.append('\n');
                return STATUS_OK;
            }

            virtual status_t include(sfz::PullParser *parser, const char *name)
            {
                sData.fmt_append_ascii("// #include \"%s\"\n", name);
                for (size_t i=0, n=vFiles.size(); i<n; ++i)
                {
                    pair_t *p = vFiles.uget(i);
                    if (strcmp(p->key, name) == 0)
                        return parser->wrap(p->value);
                }

                return STATUS_NOT_FOUND;
            }

            virtual const char *root_file_name() override
            {
                return "test.sfz";
            }

            virtual status_t end(status_t result)
            {
                sData.append_ascii("// end\n");
                return STATUS_OK;
            }
    };

    void check_namespaces_simple()
    {
        static const char *data =
            "<global>\n"
            "loop_mode=one_shot\n"
            "ampeg_attack=0.001\n"
            "ampeg_decay=0.7\n"
            "ampeg_sustain=100\n"
            "\n"
            "<master>\n"
            "amplitude_cc30=100\n"
            "offset_cc33=3000\n"
            "ampeg_sustain_oncc33=-100\n"
            "<group> key=36\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr1.wav hirand=0.250\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr2.wav lorand=0.250 hirand=0.500\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr3.wav lorand=0.500 hirand=0.750\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr4.wav lorand=0.750\n"
            "\n"
            "<master>\n"
            "amplitude_cc35=100\n"
            "offset_cc38=1500\n"
            "ampeg_sustain_oncc38=-100\n"
            "<group>key=38\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr1.wav hirand=0.250\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr2.wav lorand=0.250 hirand=0.500\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr3.wav lorand=0.500 hirand=0.750\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr4.wav lorand=0.750\n";

        static const char *expected =
            "// begin\n"
            "<region>\n"
            "ampeg_attack=0.001 ampeg_decay=0.7 ampeg_sustain=100 ampeg_sustain_oncc33=-100 amplitude_cc30=100 hirand=0.250 key=36 loop_mode=one_shot offset_cc33=3000 sample=../Samples/bobobo/bobobo_bass_vl1_rr1.wav\n"
            "<region>\n"
            "ampeg_attack=0.001 ampeg_decay=0.7 ampeg_sustain=100 ampeg_sustain_oncc33=-100 amplitude_cc30=100 hirand=0.500 key=36 loop_mode=one_shot lorand=0.250 offset_cc33=3000 sample=../Samples/bobobo/bobobo_bass_vl1_rr2.wav\n"
            "<region>\n"
            "ampeg_attack=0.001 ampeg_decay=0.7 ampeg_sustain=100 ampeg_sustain_oncc33=-100 amplitude_cc30=100 hirand=0.750 key=36 loop_mode=one_shot lorand=0.500 offset_cc33=3000 sample=../Samples/bobobo/bobobo_bass_vl1_rr3.wav\n"
            "<region>\n"
            "ampeg_attack=0.001 ampeg_decay=0.7 ampeg_sustain=100 ampeg_sustain_oncc33=-100 amplitude_cc30=100 key=36 loop_mode=one_shot lorand=0.750 offset_cc33=3000 sample=../Samples/bobobo/bobobo_bass_vl1_rr4.wav\n"
            "<region>\n"
            "ampeg_attack=0.001 ampeg_decay=0.7 ampeg_sustain=100 ampeg_sustain_oncc38=-100 amplitude_cc35=100 hirand=0.250 key=38 loop_mode=one_shot offset_cc38=1500 sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr1.wav\n"
            "<region>\n"
            "ampeg_attack=0.001 ampeg_decay=0.7 ampeg_sustain=100 ampeg_sustain_oncc38=-100 amplitude_cc35=100 hirand=0.500 key=38 loop_mode=one_shot lorand=0.250 offset_cc38=1500 sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr2.wav\n"
            "<region>\n"
            "ampeg_attack=0.001 ampeg_decay=0.7 ampeg_sustain=100 ampeg_sustain_oncc38=-100 amplitude_cc35=100 hirand=0.750 key=38 loop_mode=one_shot lorand=0.500 offset_cc38=1500 sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr3.wav\n"
            "<region>\n"
            "ampeg_attack=0.001 ampeg_decay=0.7 ampeg_sustain=100 ampeg_sustain_oncc38=-100 amplitude_cc35=100 key=38 loop_mode=one_shot lorand=0.750 offset_cc38=1500 sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr4.wav\n"
            "// end\n";

        printf("Checking namespace parameter merging...\n");

        SFZHandler handler;
        sfz::DocumentProcessor processor;
        UTEST_ASSERT(processor.wrap(data) == STATUS_OK);
        UTEST_ASSERT(processor.process(&handler) == STATUS_OK);
        UTEST_ASSERT(processor.close() == STATUS_OK);

        const char *processed = handler.data()->get_utf8();

        printf("Source document:\n%s\n", data);
        printf("Processed document:\n%s\n", processed);
        if (strcmp(processed, expected) != 0)
            UTEST_FAIL_MSG("Expected document:\n%s\n", expected);
    }

    void check_namespaces_overrides()
    {
        static const char *data =
            "<control>\n"
            "default_path=../Samples/bobobo/ note_offset=-3\n"
            "<global>\n"
            "loop_mode=one_shot\n"
            "ampeg_attack=0.001\n"
            "ampeg_decay=0.7\n"
            "ampeg_sustain=100\n"
            "\n"
            "<master>\n"
            "amplitude_cc30=100\n"
            "offset_cc33=3000\n"
            "ampeg_sustain_oncc33=-100\n"
            "<group> key=36\n"
            "<region>\n"
            "sample=bobobo_bass_vl1_rr1.wav hirand=0.250 loop_mode=no_loop key=37\n"
            "<region>\n"
            "sample=bobobo_bass_vl1_rr2.wav lorand=0.250 hirand=0.500 ampeg_attack=0.1 offset_cc33=2000\n"
            "<region>\n"
            "sample=bobobo_bass_vl1_rr3.wav lorand=0.500 hirand=0.750 ampeg_attack=0.1\n"
            "<region>\n"
            "sample=bobobo_bass_vl1_rr4.wav lorand=0.750 amplitude_cc30=50 key=37\n"
            "\n"
            "<master>\n"
            "amplitude_cc35=100\n"
            "offset_cc38=1500\n"
            "ampeg_sustain_oncc38=-100\n"
            "<group>key=38\n"
            "<region>\n"
            "sample=bobobo_tenor_l_vl1_rr1.wav hirand=0.250 ampeg_attack=0.1\n"
            "<region>\n"
            "sample=bobobo_tenor_l_vl1_rr2.wav lorand=0.250 hirand=0.500 key=39 offset_cc38=2000\n"
            "<region>\n"
            "sample=bobobo_tenor_l_vl1_rr3.wav lorand=0.500 hirand=0.750 key=39 offset_cc38=4000\n"
            "<region>\n"
            "sample=bobobo_tenor_l_vl1_rr4.wav lorand=0.750 loop_mode=continuous amplitude_cc35=50\n";

        static const char *expected =
            "// begin\n"
            "<control>\n"
            "default_path=../Samples/bobobo/ note_offset=-3\n"
            "<region>\n"
            "ampeg_attack=0.001 ampeg_decay=0.7 ampeg_sustain=100 ampeg_sustain_oncc33=-100 amplitude_cc30=100 hirand=0.250 key=37 loop_mode=no_loop offset_cc33=3000 sample=bobobo_bass_vl1_rr1.wav\n"
            "<region>\n"
            "ampeg_attack=0.1 ampeg_decay=0.7 ampeg_sustain=100 ampeg_sustain_oncc33=-100 amplitude_cc30=100 hirand=0.500 key=36 loop_mode=one_shot lorand=0.250 offset_cc33=2000 sample=bobobo_bass_vl1_rr2.wav\n"
            "<region>\n"
            "ampeg_attack=0.1 ampeg_decay=0.7 ampeg_sustain=100 ampeg_sustain_oncc33=-100 amplitude_cc30=100 hirand=0.750 key=36 loop_mode=one_shot lorand=0.500 offset_cc33=3000 sample=bobobo_bass_vl1_rr3.wav\n"
            "<region>\n"
            "ampeg_attack=0.001 ampeg_decay=0.7 ampeg_sustain=100 ampeg_sustain_oncc33=-100 amplitude_cc30=50 key=37 loop_mode=one_shot lorand=0.750 offset_cc33=3000 sample=bobobo_bass_vl1_rr4.wav\n"
            "<region>\n"
            "ampeg_attack=0.1 ampeg_decay=0.7 ampeg_sustain=100 ampeg_sustain_oncc38=-100 amplitude_cc35=100 hirand=0.250 key=38 loop_mode=one_shot offset_cc38=1500 sample=bobobo_tenor_l_vl1_rr1.wav\n"
            "<region>\n"
            "ampeg_attack=0.001 ampeg_decay=0.7 ampeg_sustain=100 ampeg_sustain_oncc38=-100 amplitude_cc35=100 hirand=0.500 key=39 loop_mode=one_shot lorand=0.250 offset_cc38=2000 sample=bobobo_tenor_l_vl1_rr2.wav\n"
            "<region>\n"
            "ampeg_attack=0.001 ampeg_decay=0.7 ampeg_sustain=100 ampeg_sustain_oncc38=-100 amplitude_cc35=100 hirand=0.750 key=39 loop_mode=one_shot lorand=0.500 offset_cc38=4000 sample=bobobo_tenor_l_vl1_rr3.wav\n"
            "<region>\n"
            "ampeg_attack=0.001 ampeg_decay=0.7 ampeg_sustain=100 ampeg_sustain_oncc38=-100 amplitude_cc35=50 key=38 loop_mode=continuous lorand=0.750 offset_cc38=1500 sample=bobobo_tenor_l_vl1_rr4.wav\n"
            "// end\n";

        printf("Checking namespace parameter overriding...\n");

        SFZHandler handler;
        sfz::DocumentProcessor processor;
        UTEST_ASSERT(processor.process(&handler) == STATUS_CLOSED);
        UTEST_ASSERT(processor.wrap(data) == STATUS_OK);
        UTEST_ASSERT(processor.process(&handler) == STATUS_OK);
        UTEST_ASSERT(processor.close() == STATUS_OK);
        UTEST_ASSERT(processor.process(&handler) == STATUS_CLOSED);

        const char *processed = handler.data()->get_utf8();

        printf("Source document:\n%s\n", data);
        printf("Processed document:\n%s\n", processed);
        if (strcmp(processed, expected) != 0)
            UTEST_FAIL_MSG("Expected document:\n%s\n", expected);
    }

    void check_includes_and_defines()
    {
        static const char *data1 =
            "<control>\n"
            "default_path=../Samples/bobobo/\n"
            "#define $KEY1 36\n"
            "#define $KEY2 37\n"
            "#define $KEY3 38\n"
            "#define $KEY4 39\n"
            "<master>\n"
            "amplitude_cc30=100\n"
            "offset_cc33=3000\n"
            "ampeg_sustain_oncc33=-100\n"
            "<group> key=$KEY1\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr1.wav hirand=0.250\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr2.wav lorand=0.250 hirand=0.500\n"
            "<group> key=$KEY2\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr3.wav lorand=0.500 hirand=0.750\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr4.wav lorand=0.750\n"
            "\n"
            "#include \"other-config.sfz\"\n"
            "<master>\n"
            "amplitude_cc35=100\n"
            "offset_cc38=1500\n"
            "ampeg_sustain_oncc38=-100\n"
            "<group>key=$KEY3\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr1.wav hirand=0.250\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr2.wav lorand=0.250 hirand=0.500\n"
            "<group>key=$KEY3\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr3.wav lorand=0.500 hirand=0.750\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr4.wav lorand=0.750\n";

        static const char *data2 =
            "<control>\n"
            "default_path=../Samples/bobobo/\n"
            "#define $KEY1 40\n"
            "#define $KEY2 41\n"
            "#define $KEY3 42\n"
            "#define $KEY4 43\n"
            "<master>\n"
            "amplitude_cc30=100\n"
            "offset_cc33=3000\n"
            "ampeg_sustain_oncc33=-100\n"
            "<group> key=$KEY1\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr1.wav hirand=0.250\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr2.wav lorand=0.250 hirand=0.500\n"
            "<group> key=$KEY2\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr3.wav lorand=0.500 hirand=0.750\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr4.wav lorand=0.750\n"
            "\n"
            "<master>\n"
            "amplitude_cc35=100\n"
            "offset_cc38=1500\n"
            "ampeg_sustain_oncc38=-100\n"
            "<group>key=$KEY3\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr1.wav hirand=0.250\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr2.wav lorand=0.250 hirand=0.500\n"
            "<group>key=$KEY4\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr3.wav lorand=0.500 hirand=0.750\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr4.wav lorand=0.750\n";

        static const char *expected =
            "// begin\n"
            "<control>\n"
            "default_path=../Samples/bobobo/\n"
            "<region>\n"
            "ampeg_sustain_oncc33=-100 amplitude_cc30=100 hirand=0.250 key=36 offset_cc33=3000 sample=../Samples/bobobo/bobobo_bass_vl1_rr1.wav\n"
            "<region>\n"
            "ampeg_sustain_oncc33=-100 amplitude_cc30=100 hirand=0.500 key=36 lorand=0.250 offset_cc33=3000 sample=../Samples/bobobo/bobobo_bass_vl1_rr2.wav\n"
            "<region>\n"
            "ampeg_sustain_oncc33=-100 amplitude_cc30=100 hirand=0.750 key=37 lorand=0.500 offset_cc33=3000 sample=../Samples/bobobo/bobobo_bass_vl1_rr3.wav\n"
            "// #include \"other-config.sfz\"\n"
            "<region>\n"
            "ampeg_sustain_oncc33=-100 amplitude_cc30=100 key=37 lorand=0.750 offset_cc33=3000 sample=../Samples/bobobo/bobobo_bass_vl1_rr4.wav\n"
            "<control>\n"
            "default_path=../Samples/bobobo/\n"
            "<region>\n"
            "ampeg_sustain_oncc33=-100 amplitude_cc30=100 hirand=0.250 key=40 offset_cc33=3000 sample=../Samples/bobobo/bobobo_bass_vl1_rr1.wav\n"
            "<region>\n"
            "ampeg_sustain_oncc33=-100 amplitude_cc30=100 hirand=0.500 key=40 lorand=0.250 offset_cc33=3000 sample=../Samples/bobobo/bobobo_bass_vl1_rr2.wav\n"
            "<region>\n"
            "ampeg_sustain_oncc33=-100 amplitude_cc30=100 hirand=0.750 key=41 lorand=0.500 offset_cc33=3000 sample=../Samples/bobobo/bobobo_bass_vl1_rr3.wav\n"
            "<region>\n"
            "ampeg_sustain_oncc33=-100 amplitude_cc30=100 key=41 lorand=0.750 offset_cc33=3000 sample=../Samples/bobobo/bobobo_bass_vl1_rr4.wav\n"
            "<control>\n"
            "default_path=../Samples/bobobo/\n"
            "<region>\n"
            "ampeg_sustain_oncc38=-100 amplitude_cc35=100 hirand=0.250 key=42 offset_cc38=1500 sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr1.wav\n"
            "<region>\n"
            "ampeg_sustain_oncc38=-100 amplitude_cc35=100 hirand=0.500 key=42 lorand=0.250 offset_cc38=1500 sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr2.wav\n"
            "<region>\n"
            "ampeg_sustain_oncc38=-100 amplitude_cc35=100 hirand=0.750 key=43 lorand=0.500 offset_cc38=1500 sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr3.wav\n"
            "<region>\n"
            "ampeg_sustain_oncc38=-100 amplitude_cc35=100 key=43 lorand=0.750 offset_cc38=1500 sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr4.wav\n"
            "<control>\n"
            "default_path=../Samples/bobobo/\n"
            "<region>\n"
            "ampeg_sustain_oncc38=-100 amplitude_cc35=100 hirand=0.250 key=38 offset_cc38=1500 sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr1.wav\n"
            "<region>\n"
            "ampeg_sustain_oncc38=-100 amplitude_cc35=100 hirand=0.500 key=38 lorand=0.250 offset_cc38=1500 sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr2.wav\n"
            "<region>\n"
            "ampeg_sustain_oncc38=-100 amplitude_cc35=100 hirand=0.750 key=38 lorand=0.500 offset_cc38=1500 sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr3.wav\n"
            "<region>\n"
            "ampeg_sustain_oncc38=-100 amplitude_cc35=100 key=38 lorand=0.750 offset_cc38=1500 sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr4.wav\n"
            "// end\n";

        printf("Checking nested file includes...\n");

        SFZHandler handler;
        handler.add_file("other-config.sfz", data2);

        sfz::DocumentProcessor processor;
        UTEST_ASSERT(processor.wrap(data1) == STATUS_OK);
        UTEST_ASSERT(processor.process(&handler) == STATUS_OK);
        UTEST_ASSERT(processor.close() == STATUS_OK);

        const char *processed = handler.data()->get_utf8();

        printf("Source document 1:\n%s\n", data1);
        printf("Source document 2:\n%s\n", data2);
        printf("Processed document:\n%s\n", processed);
        if (strcmp(processed, expected) != 0)
            UTEST_FAIL_MSG("Expected document:\n%s\n", expected);
    }

    void check_other_headers()
    {
        static const char *data =
            "<sample> name=sample1.wav\n"
            "some_opcode1=value1\n"
            "data=gggJ~\x92\x93\x9dJ\x93\x9dJ\x8bJ\x9e\x8f\x9d\x9eJlvylJggg7444oyp7474$\r\n"
            "some_opcode2=value2\n"
            "<sample> name=sample2.wav\n"
            "some_opcode1=value1\n"
            "data=gggJ~\x92\x93\x9dJ\x94\x9eJ\x8bJ\x9e\x8f\x9d\x9eJlvylJggg7444oyp7474$\r\n"
            "<curve>curve_index=17\n"
            "v000=0\n"
            "v095=1\n"
            "v127=1\n"
            "\n"
            "<curve>curve_index=18\n"
            "v000=0\n"
            "v095=0.5\n"
            "v127=1\n"
            "<magic> param1=value1 param2=value2\n"
            "<region> amplitude_curvecc110=9 sample=sample1.wav pitch_keycenter=69\r\n"
            "<region> amplitude_curvecc110=10 sample=sample2.wav pitch_keycenter=80\r\n";

        static const char *expected =
            "// begin\n"
            "<sample>\n"
            "name=sample1.wav data=0x3b61a865 some_opcode1=value1 some_opcode2=value2\n"
            "<sample>\n"
            "name=sample2.wav data=0x5b61a977 some_opcode1=value1\n"
            "<curve>\n"
            "curve_index=17 v000=0 v095=1 v127=1\n"
            "<curve>\n"
            "curve_index=18 v000=0 v095=0.5 v127=1\n"
            "<magic>\n"
            "param1=value1 param2=value2\n"
            "<region>\n"
            "amplitude_curvecc110=9 pitch_keycenter=69 sample=sample1.wav\n"
            "<region>\n"
            "amplitude_curvecc110=10 pitch_keycenter=80 sample=sample2.wav\n"
            "// end\n";

        SFZHandler handler;
        sfz::DocumentProcessor processor;
        UTEST_ASSERT(processor.wrap(data) == STATUS_OK);
        UTEST_ASSERT(processor.process(&handler) == STATUS_OK);
        UTEST_ASSERT(processor.close() == STATUS_OK);

        const char *processed = handler.data()->get_utf8();

        printf("Source document:\n%s\n", data);
        printf("Processed document:\n%s\n", processed);
        if (strcmp(processed, expected) != 0)
            UTEST_FAIL_MSG("Expected document:\n%s\n", expected);
    }

    void check_read_file()
    {
        static const char *expected =
            "// begin\n"
            "<region>\n"
            "amp_velcurve_31=1 hivel=31 key=36 loop_mode=one_shot sample=kick_vl1_rr1.wav seq_length=4 seq_position=1\n"
            "<region>\n"
            "amp_velcurve_31=1 hivel=31 key=36 loop_mode=one_shot sample=kick_vl1_rr2.wav seq_length=4 seq_position=2\n"
            "<region>\n"
            "amp_velcurve_31=1 hivel=31 key=36 loop_mode=one_shot sample=kick_vl1_rr3.wav seq_length=4 seq_position=3\n"
            "<region>\n"
            "amp_velcurve_31=1 hivel=31 key=36 loop_mode=one_shot sample=kick_vl1_rr4.wav seq_length=4 seq_position=4\n"
            "<region>\n"
            "amp_velcurve_63=1 hivel=63 key=36 loop_mode=one_shot lovel=32 sample=kick_vl2_rr1.wav seq_length=4 seq_position=1\n"
            "<region>\n"
            "amp_velcurve_63=1 hivel=63 key=36 loop_mode=one_shot lovel=32 sample=kick_vl2_rr2.wav seq_length=4 seq_position=2\n"
            "<region>\n"
            "amp_velcurve_63=1 hivel=63 key=36 loop_mode=one_shot lovel=32 sample=kick_vl2_rr3.wav seq_length=4 seq_position=3\n"
            "<region>\n"
            "amp_velcurve_63=1 hivel=63 key=36 loop_mode=one_shot lovel=32 sample=kick_vl2_rr4.wav seq_length=4 seq_position=4\n"
            "<region>\n"
            "amp_velcurve_95=1 hivel=95 key=36 loop_mode=one_shot lovel=64 sample=kick_vl3_rr1.wav seq_length=4 seq_position=1\n"
            "<region>\n"
            "amp_velcurve_95=1 hivel=95 key=36 loop_mode=one_shot lovel=64 sample=kick_vl3_rr2.wav seq_length=4 seq_position=2\n"
            "<region>\n"
            "amp_velcurve_95=1 hivel=95 key=36 loop_mode=one_shot lovel=64 sample=kick_vl3_rr3.wav seq_length=4 seq_position=3\n"
            "<region>\n"
            "amp_velcurve_95=1 hivel=95 key=36 loop_mode=one_shot lovel=64 sample=kick_vl3_rr4.wav seq_length=4 seq_position=4\n"
            "<region>\n"
            "key=36 loop_mode=one_shot lovel=96 sample=kick_vl4_rr1.wav seq_length=3 seq_position=1\n"
            "<region>\n"
            "key=36 loop_mode=one_shot lovel=96 sample=kick_vl4_rr2.wav seq_length=3 seq_position=2\n"
            "<region>\n"
            "key=36 loop_mode=one_shot lovel=96 sample=kick_vl4_rr3.wav seq_length=3 seq_position=3\n"
            "// end\n";

        io::Path path;
        UTEST_ASSERT(path.fmt("%s/fmt/sfz/example.sfz", resources()) > 0);

        printf("Checking read of the file %s...\n", path.get());

        SFZHandler handler;
        sfz::DocumentProcessor processor;
        UTEST_ASSERT(processor.open(&path) == STATUS_OK);
        UTEST_ASSERT(processor.process(&handler) == STATUS_OK);
        UTEST_ASSERT(processor.close() == STATUS_OK);

        const char *processed = handler.data()->get_utf8();

        printf("Processed document:\n%s\n", processed);
        if (strcmp(processed, expected) != 0)
            UTEST_FAIL_MSG("Expected document:\n%s\n", expected);
    }

    void check_invalid_include()
    {
        static const char *data1 =
            "<master>\n"
            "<group> key=38\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr1.wav hirand=0.250\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr2.wav lorand=0.250 hirand=0.500\n"
            "#include \"\""
            "<group> key=39\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr3.wav lorand=0.500 hirand=0.750\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr4.wav lorand=0.750\n"
            "\n";

        printf("Checking invalid file includes...\n");

        SFZHandler handler;
        sfz::DocumentProcessor processor;
        UTEST_ASSERT(processor.wrap(data1) == STATUS_OK);
        UTEST_ASSERT(processor.process(&handler) == STATUS_NOT_FOUND);
        UTEST_ASSERT(processor.close() == STATUS_OK);
    }

    void check_recursive_include_root()
    {
        static const char *data1 =
            "<master>\n"
            "<group> key=38\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr1.wav hirand=0.250\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr2.wav lorand=0.250 hirand=0.500\n"
            "#include \"test.sfz\""
            "<group> key=39\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr3.wav lorand=0.500 hirand=0.750\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr4.wav lorand=0.750\n"
            "\n";

        printf("Checking recursive include of the root file...\n");

        SFZHandler handler;
        sfz::DocumentProcessor processor;
        UTEST_ASSERT(processor.wrap(data1) == STATUS_OK);
        UTEST_ASSERT(processor.process(&handler) == STATUS_OVERFLOW);
        UTEST_ASSERT(processor.close() == STATUS_OK);
    }

    void check_recursive_include_loop()
    {
        static const char *file0 =
            "<master>\n"
            "<group> key=38\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr1.wav hirand=0.250\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr2.wav lorand=0.250 hirand=0.500\n"
            "#include \"file1.sfz\""
            "<group> key=39\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr3.wav lorand=0.500 hirand=0.750\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr4.wav lorand=0.750\n"
            "\n";

        static const char *file1 =
            "<master>\n"
            "<group> key=40\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr1.wav hirand=0.250\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr2.wav lorand=0.250 hirand=0.500\n"
            "#include \"file2.sfz\""
            "<group> key=41\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr3.wav lorand=0.500 hirand=0.750\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr4.wav lorand=0.750\n"
            "\n";

        static const char *file2 =
            "<master>\n"
            "<group> key=42\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr1.wav hirand=0.250\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr2.wav lorand=0.250 hirand=0.500\n"
            "#include \"file3.sfz\""
            "<group> key=43\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr3.wav lorand=0.500 hirand=0.750\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr4.wav lorand=0.750\n"
            "\n";

        static const char *file3 =
            "<master>\n"
            "<group> key=44\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr1.wav hirand=0.250\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr2.wav lorand=0.250 hirand=0.500\n"
            "#include \"file1.sfz\""
            "<group> key=45\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr3.wav lorand=0.500 hirand=0.750\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_bass_vl1_rr4.wav lorand=0.750\n"
            "\n";

        printf("Checking recursive include loop...\n");

        SFZHandler handler;
        handler.add_file("file1.sfz", file1);
        handler.add_file("file2.sfz", file2);
        handler.add_file("file3.sfz", file3);

        sfz::DocumentProcessor processor;
        UTEST_ASSERT(processor.wrap(file0) == STATUS_OK);
        UTEST_ASSERT(processor.process(&handler) == STATUS_OVERFLOW);
        UTEST_ASSERT(processor.close() == STATUS_OK);
    }


    UTEST_MAIN
    {
        check_namespaces_simple();
        check_namespaces_overrides();
        check_includes_and_defines();
        check_other_headers();
        check_read_file();
        check_invalid_include();
        check_recursive_include_root();
        check_recursive_include_loop();
    }

UTEST_END






