/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 14 апр. 2020 г.
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

#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/protocol/midi.h>
#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/test-fw/ByteBuffer.h>

namespace lsp
{
    static const uint8_t message[]=
    {
        0x9c, 0x3e, 0x3c,   // Note on: channel = 0x0c, note = 0x3e, velocity = 0x3c
        0x83, 0x5a, 0x45,   // Note off: channel = 0x03, note = 0x5a, velocity = 0x45
        0xb2, 0x08, 0x7f,   // Controller: channel = 0x02, balance msb = 0x7f
        0xb2, 0x28, 0x7e,   // Controller: channel = 0x02, balance lsb = 0x7e
        0xb4, 0x40, 0x0f,   // Controller: channel = 0x04, sustain = 0x0f
        0xf1, 0x5c,         // MTC Quarter: type = 0x05, value = 0x0c
        0xae, 0x40, 0x44,   // Aftertourch: channel = 0x0e, note = 0x40, velocity = 0x44
        0xc3, 0x63,         // Program change: channel = 0x03, program = 0x63
        0xd8, 0x55,         // Channel pressure: channel = 0x08, pressure = 0x55
        0xe7, 0x3c, 0x22,   // Pitch bend: channel = 0x07, bend = 0x113c
        0xf2, 0x1e, 0x22,   // Song position select: position = 0x111e
        0xf3, 0x42,         // Song select: song = 0x42
        0xf8,               // MIDI Clock
        0x00                // Invalid message
    };
}

UTEST_BEGIN("runtime.protocol", midi)
    void test_decode()
    {
        midi::event_t ev;
        const uint8_t *b = message;

        UTEST_ASSERT(midi::decode(&ev, b) == 3);
        UTEST_ASSERT((ev.type == midi::MIDI_MSG_NOTE_ON));
        UTEST_ASSERT((ev.channel == 0x0c) && (ev.note.pitch == 0x3e) && (ev.note.velocity == 0x3c));
        b += 3;

        UTEST_ASSERT(midi::decode(&ev, b) == 3);
        UTEST_ASSERT((ev.type == midi::MIDI_MSG_NOTE_OFF));
        UTEST_ASSERT((ev.channel == 0x03) && (ev.note.pitch == 0x5a) && (ev.note.velocity == 0x45));
        b += 3;

        UTEST_ASSERT(midi::decode(&ev, b) == 3);
        UTEST_ASSERT((ev.type == midi::MIDI_MSG_NOTE_CONTROLLER));
        UTEST_ASSERT((ev.channel == 0x02) && (ev.ctl.control == midi::MIDI_CTL_MSB_BALANCE) && (ev.ctl.value == 0x7f));
        b += 3;

        UTEST_ASSERT(midi::decode(&ev, b) == 3);
        UTEST_ASSERT((ev.type == midi::MIDI_MSG_NOTE_CONTROLLER));
        UTEST_ASSERT((ev.channel == 0x02) && (ev.ctl.control == midi::MIDI_CTL_LSB_BALANCE) && (ev.ctl.value == 0x7e));
        b += 3;

        UTEST_ASSERT(midi::decode(&ev, b) == 3);
        UTEST_ASSERT((ev.type == midi::MIDI_MSG_NOTE_CONTROLLER));
        UTEST_ASSERT((ev.channel == 0x04) && (ev.ctl.control == midi::MIDI_CTL_SUSTAIN) && (ev.ctl.value == 0x0f));
        b += 3;

        UTEST_ASSERT(midi::decode(&ev, b) == 2);
        UTEST_ASSERT((ev.type == midi::MIDI_MSG_MTC_QUARTER));
        UTEST_ASSERT((ev.mtc.type == 0x05) && (ev.mtc.value == 0x0c));
        b += 2;

        UTEST_ASSERT(midi::decode(&ev, b) == 3);
        UTEST_ASSERT((ev.type == midi::MIDI_MSG_NOTE_PRESSURE));
        UTEST_ASSERT((ev.channel == 0x0e) && (ev.atouch.pitch == 0x40) && (ev.atouch.pressure = 0x44));
        b += 3;

        UTEST_ASSERT(midi::decode(&ev, b) == 2);
        UTEST_ASSERT((ev.type == midi::MIDI_MSG_PROGRAM_CHANGE));
        UTEST_ASSERT((ev.channel == 0x03) && (ev.program == 0x63));
        b += 2;

        UTEST_ASSERT(midi::decode(&ev, b) == 2);
        UTEST_ASSERT((ev.type == midi::MIDI_MSG_CHANNEL_PRESSURE));
        UTEST_ASSERT((ev.channel == 0x08) && (ev.chn.pressure = 0x55));
        b += 2;

        UTEST_ASSERT(midi::decode(&ev, b) == 3);
        UTEST_ASSERT((ev.type == midi::MIDI_MSG_PITCH_BEND));
        UTEST_ASSERT((ev.channel == 0x07) && (ev.bend = 0x113c));
        b += 3;

        UTEST_ASSERT(midi::decode(&ev, b) == 3);
        UTEST_ASSERT((ev.type == midi::MIDI_MSG_SONG_POS));
        UTEST_ASSERT((ev.beats = 0x111e));
        b += 3;

        UTEST_ASSERT(midi::decode(&ev, b) == 2);
        UTEST_ASSERT((ev.type == midi::MIDI_MSG_SONG_SELECT));
        UTEST_ASSERT((ev.song = 0x42));
        b += 2;

        UTEST_ASSERT(midi::decode(&ev, b) == 1);
        UTEST_ASSERT((ev.type == midi::MIDI_MSG_CLOCK));
        b += 1;

        UTEST_ASSERT(midi::decode(&ev, b) == -STATUS_CORRUPTED);
    }

    void test_encode()
    {
        uint8_t buf[0x100];

        midi::event_t ev;
        uint8_t *b = buf;

        ev.type             = midi::MIDI_MSG_NOTE_ON;
        ev.channel          = 0x0c;
        ev.note.pitch       = 0x3e;
        ev.note.velocity    = 0x3c;
        UTEST_ASSERT(midi::size_of(&ev) == 3);
        UTEST_ASSERT(midi::encode(b, &ev) == 3);
        b += 3;

        ev.type             = midi::MIDI_MSG_NOTE_OFF;
        ev.channel          = 0x03;
        ev.note.pitch       = 0x5a;
        ev.note.velocity    = 0x45;
        UTEST_ASSERT(midi::size_of(&ev) == 3);
        UTEST_ASSERT(midi::encode(b, &ev) == 3);
        b += 3;

        ev.type             = midi::MIDI_MSG_NOTE_CONTROLLER;
        ev.channel          = 0x02;
        ev.ctl.control      = midi::MIDI_CTL_MSB_BALANCE;
        ev.ctl.value        = 0x7f;
        UTEST_ASSERT(midi::size_of(&ev) == 3);
        UTEST_ASSERT(midi::encode(b, &ev) == 3);
        b += 3;

        ev.type             = midi::MIDI_MSG_NOTE_CONTROLLER;
        ev.channel          = 0x02;
        ev.ctl.control      = midi::MIDI_CTL_LSB_BALANCE;
        ev.ctl.value        = 0x7e;
        UTEST_ASSERT(midi::size_of(&ev) == 3);
        UTEST_ASSERT(midi::encode(b, &ev) == 3);
        b += 3;

        ev.type             = midi::MIDI_MSG_NOTE_CONTROLLER;
        ev.channel          = 0x04;
        ev.ctl.control      = midi::MIDI_CTL_SUSTAIN;
        ev.ctl.value        = 0x0f;
        UTEST_ASSERT(midi::size_of(&ev) == 3);
        UTEST_ASSERT(midi::encode(b, &ev) == 3);
        b += 3;

        ev.type             = midi::MIDI_MSG_MTC_QUARTER;
        ev.mtc.type         = 0x05;
        ev.mtc.value        = 0x0c;
        UTEST_ASSERT(midi::size_of(&ev) == 2);
        UTEST_ASSERT(midi::encode(b, &ev) == 2);
        b += 2;

        ev.type             = midi::MIDI_MSG_NOTE_PRESSURE;
        ev.channel          = 0x0e;
        ev.atouch.pitch     = 0x40;
        ev.atouch.pressure  = 0x44;
        UTEST_ASSERT(midi::size_of(&ev) == 3);
        UTEST_ASSERT(midi::encode(b, &ev) == 3);
        b += 3;

        ev.type             = midi::MIDI_MSG_PROGRAM_CHANGE;
        ev.channel          = 0x03;
        ev.program          = 0x63;
        UTEST_ASSERT(midi::size_of(&ev) == 2);
        UTEST_ASSERT(midi::encode(b, &ev) == 2);
        b += 2;

        ev.type             = midi::MIDI_MSG_CHANNEL_PRESSURE;
        ev.channel          = 0x08;
        ev.chn.pressure     = 0x55;
        UTEST_ASSERT(midi::size_of(&ev) == 2);
        UTEST_ASSERT(midi::encode(b, &ev) == 2);
        b += 2;

        ev.type             = midi::MIDI_MSG_PITCH_BEND;
        ev.channel          = 0x07;
        ev.bend             = 0x113c;
        UTEST_ASSERT(midi::size_of(&ev) == 3);
        UTEST_ASSERT(midi::encode(b, &ev) == 3);
        b += 3;

        ev.type             = midi::MIDI_MSG_SONG_POS;
        ev.beats            = 0x111e;
        UTEST_ASSERT(midi::size_of(&ev) == 3);
        UTEST_ASSERT(midi::encode(b, &ev) == 3);
        b += 3;

        ev.type             = midi::MIDI_MSG_SONG_SELECT;
        ev.song             = 0x42;
        UTEST_ASSERT(midi::size_of(&ev) == 2);
        UTEST_ASSERT(midi::encode(b, &ev) == 2);
        b += 2;

        ev.type             = midi::MIDI_MSG_CLOCK;
        UTEST_ASSERT(midi::size_of(&ev) == 1);
        UTEST_ASSERT(midi::encode(b, &ev) == 1);
        b += 1;

        if (::memcmp(buf, message, b-buf) != 0)
        {
            ByteBuffer b1(message, b-buf);
            ByteBuffer b2(buf, b-buf);

            b1.dump("msg");
            b2.dump("buf");
            UTEST_FAIL_MSG("Failed comparison of byte buffers");
        }
    }

    UTEST_MAIN
    {
        #define CALL(v) printf("Executing " #v "...\n"); v();

        CALL(test_decode);
        CALL(test_encode);
    }
UTEST_END;



