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
                sData.fmt_append_ascii("name=%s data=0x%08x", int(hash_data(data)));
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
        const char *data =
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

        const char *expected =
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
        const char *data =
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

        const char *expected =
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

    UTEST_MAIN
    {
        check_namespaces_simple();
        check_namespaces_overrides();
    }

UTEST_END






