/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 30 янв. 2023 г.
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

#include <lsp-plug.in/fmt/sfz/PullParser.h>
#include <lsp-plug.in/test-fw/utest.h>

UTEST_BEGIN("runtime.fmt.sfz", pullparser)

    class Verifier
    {
        private:
            sfz::PullParser     sParser;
            test_type_t        *pTest;
            const char         *__test_group;
            const char         *__test_name;

        public:
            Verifier(test_type_t *test)
            {
                pTest           = test;
                __test_group    = test->__test_group;
                __test_name     = test->__test_name;
            }

        public:
            Verifier &open(const char *name)
            {
                io::Path path;
                UTEST_ASSERT(path.fmt("%s/%s", pTest->resources(), name) > 0);
                UTEST_ASSERT(sParser.open(&path) == STATUS_OK);
                return *this;
            }

            Verifier &wrap(const char *text)
            {
                UTEST_ASSERT(sParser.wrap(text) == STATUS_OK);
                return *this;
            }

            Verifier &close()
            {
                UTEST_ASSERT(sParser.close() == STATUS_OK);
                return *this;
            }

            Verifier &header(const char *name)
            {
                sfz::event_t ev;
                UTEST_ASSERT(sParser.next(&ev) == STATUS_OK);
                UTEST_ASSERT(ev.type == sfz::EVENT_HEADER);
                UTEST_ASSERT(ev.name.equals_ascii(name));
                UTEST_ASSERT(ev.value.is_empty());
                pTest->printf("  header \"%s\"\n", name);
                return *this;
            }

            Verifier &opcode(const char *name, const char *value)
            {
                sfz::event_t ev;
                UTEST_ASSERT(sParser.next(&ev) == STATUS_OK);
                UTEST_ASSERT(ev.type == sfz::EVENT_OPCODE);
                UTEST_ASSERT(ev.name.equals_ascii(name));
                UTEST_ASSERT(ev.value.equals_ascii(value));
                pTest->printf("  opcode %s=\"%s\"\n", name, value);
                return *this;
            }

            Verifier &comment(const char *value)
            {
                sfz::event_t ev;
                UTEST_ASSERT(sParser.next(&ev) == STATUS_OK);
                UTEST_ASSERT(ev.type == sfz::EVENT_COMMENT);
                UTEST_ASSERT(ev.name.is_empty());
                UTEST_ASSERT(ev.value.equals_ascii(value));
                pTest->printf("  comment \"%s\"\n", value);
                return *this;
            }

            Verifier &include(const char *value)
            {
                sfz::event_t ev;
                UTEST_ASSERT(sParser.next(&ev) == STATUS_OK);
                UTEST_ASSERT(ev.type == sfz::EVENT_INCLUDE);
                UTEST_ASSERT(ev.name.is_empty());
                UTEST_ASSERT(ev.value.equals_ascii(value));
                pTest->printf("  include \"%s\"\n", value);
                return *this;
            }

            Verifier &define(const char *name, const char *value)
            {
                sfz::event_t ev;
                UTEST_ASSERT(sParser.next(&ev) == STATUS_OK);
                UTEST_ASSERT(ev.type == sfz::EVENT_DEFINE);
                UTEST_ASSERT(ev.name.equals_ascii(name));
                UTEST_ASSERT(ev.value.equals_ascii(value));
                pTest->printf("  define %s %s\n", name, value);
                return *this;
            }

            Verifier &status(status_t code)
            {
                sfz::event_t ev;
                UTEST_ASSERT(sParser.next(&ev) == code);
                return *this;
            }
    };

    void check_valid_sfz1()
    {
        static const char *text =
            "<control>\n"
            "\n"
            "<global>\n"
            "   \n"
            "    <group>\n"
            "        <region>\n"
            "            sample=\n"
            "        <region>\n"
            "            sample=\n"
            "    <group>\n"
            "        <region>\n"
            "            sample=\n"
            "        <region>\n"
            "            sample=\n";

        printf("Checking valid SFZ (case 1)...\n");

        Verifier v(this);
        v.wrap(text);
        v.header("control");
        v.header("global");
            v.header("group");
                v.header("region");
                    v.opcode("sample", "");
                v.header("region");
                    v.opcode("sample", "");
            v.header("group");
                v.header("region");
                    v.opcode("sample", "");
                v.header("region");
                    v.opcode("sample", "");
        v.status(STATUS_EOF);
        v.close();
    }

    void check_valid_sfz2()
    {
        static const char *text =
            "<group>\r\n"
            "lovel=64 // enter stuff here if you want to apply it to all regions\r\n"
            "hivel=127\r\n"
            "\r\n"
            "<region>\r\n"
            "sample=Trumpet_C4_v2.wav\r\n"
            "key=60\r\n"
            "\r\n"
            "<region>\r\n"
            "sample=Trumpet_C#4_v2.wav\r\n"
            "key=61\r\n"
            "\r\n"
            "<region>\r\n"
            "sample=Trumpet_D4_v2.wav\r\n"
            "key=62\r\n";

        printf("Checking valid SFZ (case 2)...\n");

        Verifier v(this);
        v.wrap(text);
        v.header("group");
            v.opcode("lovel", "64");
            v.comment(" enter stuff here if you want to apply it to all regions");
            v.opcode("hivel", "127");
            v.header("region");
                v.opcode("sample", "Trumpet_C4_v2.wav");
                v.opcode("key", "60");
            v.header("region");
                v.opcode("sample", "Trumpet_C#4_v2.wav");
                v.opcode("key", "61");
            v.header("region");
                v.opcode("sample", "Trumpet_D4_v2.wav");
                v.opcode("key", "62");
        v.status(STATUS_EOF);
        v.close();
    }

    void check_valid_sfz3()
    {
        static const char *text =
            "<region> sample=piano_D4_vl1.wav lokey=62 hikey=63 pitch_keycenter=62 lovel=1 hivel=50\n"
            "<region> sample=piano_E4_vl1.wav lokey=64 hikey=65 pitch_keycenter=64 lovel=1 hivel=50\n"
            "<region> sample=piano_F#4_vl1.wav lokey=66 hikey=67 pitch_keycenter=66 lovel=11 hivel=50\n"
            "<region> sample=piano_G#4_vl1.wav lokey=68 hikey=69 pitch_keycenter=68 lovel=1 hivel=50";

        printf("Checking valid SFZ (case 3)...\n");

        Verifier v(this);
        v.wrap(text);
        v.header("region");
            v.opcode("sample", "piano_D4_vl1.wav");
            v.opcode("lokey", "62");
            v.opcode("hikey", "63");
            v.opcode("pitch_keycenter", "62");
            v.opcode("lovel", "1");
            v.opcode("hivel", "50");
        v.header("region");
            v.opcode("sample", "piano_E4_vl1.wav");
            v.opcode("lokey", "64");
            v.opcode("hikey", "65");
            v.opcode("pitch_keycenter", "64");
            v.opcode("lovel", "1");
            v.opcode("hivel", "50");
        v.header("region");
            v.opcode("sample", "piano_F#4_vl1.wav");
            v.opcode("lokey", "66");
            v.opcode("hikey", "67");
            v.opcode("pitch_keycenter", "66");
            v.opcode("lovel", "11");
            v.opcode("hivel", "50");
        v.header("region");
            v.opcode("sample", "piano_G#4_vl1.wav");
            v.opcode("lokey", "68");
            v.opcode("hikey", "69");
            v.opcode("pitch_keycenter", "68");
            v.opcode("lovel", "1");
            v.opcode("hivel", "50");
        v.status(STATUS_EOF);
        v.close();
    }

    void check_valid_sfz4()
    {
        static const char *text =
            "<control>\n"
            "#define $KEY1 36 // comment\n"
            "#define $KEY2 38\n"
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
            "<group> key=$KEY1\n"
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
            "<group>key=$KEY2\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr1.wav hirand=0.250\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr2.wav lorand=0.250 hirand=0.500\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr3.wav lorand=0.500 hirand=0.750\n"
            "<region>\n"
            "sample=../Samples/bobobo/bobobo_tenor_l_vl1_rr4.wav lorand=0.750\n";

        printf("Checking valid SFZ (case 4)...\n");

        Verifier v(this);
        v.wrap(text);
        v.header("control");
            v.define("$KEY1", "36");
            v.comment(" comment");
            v.define("$KEY2", "38");
        v.header("global");
            v.opcode("loop_mode", "one_shot");
            v.opcode("ampeg_attack", "0.001");
            v.opcode("ampeg_decay", "0.7");
            v.opcode("ampeg_sustain", "100");
            v.header("master");
                v.opcode("amplitude_cc30", "100");
                v.opcode("offset_cc33", "3000");
                v.opcode("ampeg_sustain_oncc33", "-100");
                v.header("group");
                    v.opcode("key", "$KEY1");
                    v.header("region");
                        v.opcode("sample", "../Samples/bobobo/bobobo_bass_vl1_rr1.wav");
                        v.opcode("hirand", "0.250");
                    v.header("region");
                        v.opcode("sample", "../Samples/bobobo/bobobo_bass_vl1_rr2.wav");
                        v.opcode("lorand", "0.250");
                        v.opcode("hirand", "0.500");
                    v.header("region");
                        v.opcode("sample", "../Samples/bobobo/bobobo_bass_vl1_rr3.wav");
                        v.opcode("lorand", "0.500");
                        v.opcode("hirand", "0.750");
                    v.header("region");
                        v.opcode("sample", "../Samples/bobobo/bobobo_bass_vl1_rr4.wav");
                        v.opcode("lorand", "0.750");
            v.header("master");
                v.opcode("amplitude_cc35", "100");
                v.opcode("offset_cc38", "1500");
                v.opcode("ampeg_sustain_oncc38", "-100");
                v.header("group");
                    v.opcode("key", "$KEY2");
                    v.header("region");
                        v.opcode("sample", "../Samples/bobobo/bobobo_tenor_l_vl1_rr1.wav");
                        v.opcode("hirand", "0.250");
                    v.header("region");
                        v.opcode("sample", "../Samples/bobobo/bobobo_tenor_l_vl1_rr2.wav");
                        v.opcode("lorand", "0.250");
                        v.opcode("hirand", "0.500");
                    v.header("region");
                        v.opcode("sample", "../Samples/bobobo/bobobo_tenor_l_vl1_rr3.wav");
                        v.opcode("lorand", "0.500");
                        v.opcode("hirand", "0.750");
                    v.header("region");
                        v.opcode("sample", "../Samples/bobobo/bobobo_tenor_l_vl1_rr4.wav");
                        v.opcode("lorand", "0.750");

        v.status(STATUS_EOF);
        v.close();
    }

    void check_special_cases()
    {
        static const char *text =
            "<region>\n"
            "sample=path/to//sample/1 opcode=value1\n"
            "sample=path to sample 2    opcode=value2\n"
            "#include \"some file.sfz\"\n"
            "sample=path to sample 3 // opcode=value3\n"
            "sample=path to sample 4 opcode=value4 #include \"some file 2.sfz\" // comment\n"
            "//sample=path to sample 5 opcode=value5\r\n";

        printf("Checking special cases...\n");

        Verifier v(this);
        v.wrap(text);
        v.header("region");
            v.opcode("sample", "path/to//sample/1");
            v.opcode("opcode", "value1");
            v.opcode("sample", "path to sample 2");
            v.opcode("opcode", "value2");
            v.include("some file.sfz");
            v.opcode("sample", "path to sample 3 //");
            v.opcode("opcode", "value3");
            v.opcode("sample", "path to sample 4");
            v.opcode("opcode", "value4");
            v.include("some file 2.sfz");
            v.comment(" comment");
            v.comment("sample=path to sample 5 opcode=value5");
        v.status(STATUS_EOF);
        v.close();
    }

    void check_parse_file()
    {
        printf("Checking file parse...\n");

        Verifier v(this);
        v.open("fmt/sfz/example.sfz");
        v.header("global");
            v.opcode("loop_mode", "one_shot");
            v.opcode("seq_length", "4");

            v.header("group");
                v.opcode("key", "36");
                v.opcode("hivel", "31");
                v.opcode("amp_velcurve_31", "1");

                v.header("region");
                    v.opcode("seq_position", "1");
                    v.opcode("sample", "kick_vl1_rr1.wav");

                v.header("region");
                    v.opcode("seq_position", "2");
                    v.opcode("sample", "kick_vl1_rr2.wav");

                v.header("region");
                    v.opcode("seq_position", "3");
                    v.opcode("sample", "kick_vl1_rr3.wav");

                v.header("region");
                    v.opcode("seq_position", "4");
                    v.opcode("sample", "kick_vl1_rr4.wav");

            v.header("group");
                v.opcode("key", "36");
                v.opcode("lovel", "32");
                v.opcode("hivel", "63");
                v.opcode("amp_velcurve_63", "1");

                v.header("region");
                    v.opcode("seq_position", "1");
                    v.opcode("sample", "kick_vl2_rr1.wav");

                v.header("region");
                    v.opcode("seq_position", "2");
                    v.opcode("sample", "kick_vl2_rr2.wav");

                v.header("region");
                    v.opcode("seq_position", "3");
                    v.opcode("sample", "kick_vl2_rr3.wav");

                v.header("region");
                    v.opcode("seq_position", "4");
                    v.opcode("sample", "kick_vl2_rr4.wav");

            v.header("group");
                v.opcode("key", "36");
                v.opcode("lovel", "64");
                v.opcode("hivel", "95");
                v.opcode("amp_velcurve_95", "1");

                v.header("region");
                    v.opcode("seq_position", "1");
                    v.opcode("sample", "kick_vl3_rr1.wav");

                v.header("region");
                    v.opcode("seq_position", "2");
                    v.opcode("sample", "kick_vl3_rr2.wav");

                v.header("region");
                    v.opcode("seq_position", "3");
                    v.opcode("sample", "kick_vl3_rr3.wav");

                v.header("region");
                    v.opcode("seq_position", "4");
                    v.opcode("sample", "kick_vl3_rr4.wav");

            v.header("group");
                v.opcode("key", "36");
                v.opcode("lovel", "96");
                v.opcode("seq_length", "3");

                v.header("region");
                    v.opcode("seq_position", "1");
                    v.opcode("sample", "kick_vl4_rr1.wav");

                v.header("region");
                    v.opcode("seq_position", "2");
                    v.opcode("sample", "kick_vl4_rr2.wav");

                v.header("region");
                    v.opcode("seq_position", "3");
                    v.opcode("sample", "kick_vl4_rr3.wav");

        v.status(STATUS_EOF);
        v.close();
    }

    UTEST_MAIN
    {
        check_valid_sfz1();
        check_valid_sfz2();
        check_valid_sfz3();
        check_valid_sfz4();
        check_special_cases();
        check_parse_file();
    }

UTEST_END



