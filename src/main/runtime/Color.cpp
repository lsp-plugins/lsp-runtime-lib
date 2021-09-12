/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 05 нояб. 2015 г.
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

#include <lsp-plug.in/runtime/Color.h>
#include <lsp-plug.in/stdlib/string.h>
#include <lsp-plug.in/stdlib/stdio.h>
#include <lsp-plug.in/stdlib/math.h>

#include <ctype.h>
#include <locale.h>
#include <errno.h>

namespace lsp
{
    static const float HSL_RGB_1_3          = 1.0f / 3.0f;
    static const float HSL_RGB_1_6          = 1.0f / 6.0f;
    static const float HSL_RGB_2_3          = 2.0f / 3.0f;

    static const char *skip_whitespace(const char *src, const char *end)
    {
        for ( ; src < end; ++src)
        {
            switch (*src)
            {
                case '\n':
                case '\t':
                case '\r':
                case ' ':
                    break;
                case '\0':
                    return end;
                default:
                    return src;
            }
        }

        return src;
    }

    static const char *match_prefix(const char *src, const char *end, const char *prefix)
    {
        for ( ; src < end; ++src)
        {
            if (*prefix == '\0')
                return src;
            if (tolower(*src) != *(prefix++))
                return end;
        }
        return src;
    }

    static const char *match_char(const char *src, const char *end, char ch)
    {
        if (src < end)
            return (src[0] == ch) ? &src[1] : end;
        return end;
    }

    static const char *match_chars(const char *src, const char *end, const char *allowed)
    {
        for ( ; src < end; ++src)
        {
            char c = *src;
            if (c == '\0')
                return src;

            // Lookup list of allowed characters
            for (const char *ck = allowed; *ck != '\0'; ++ck)
            {
                if (*ck == c)
                    return src;
            }
        }
        return src;
    }


    static size_t hex_char(char c)
    {
        if ((c >= '0') && (c <= '9'))
            return c - '0';
        if ((c >= 'a') && (c <= 'f'))
            return c - 'a' + 10;
        if ((c >= 'A') && (c <= 'F'))
            return c - 'A' + 10;
        return 0xff;
    }

    static const char *skip_hex(const char *ptr, const char *end)
    {
        for ( ; ptr < end; ++ptr)
        {
            if (hex_char(*ptr) >= 16)
                return ptr;
        }
        return end;
    }

    inline float Color::clamp(float x)
    {
        return lsp_limit(x, 0.0f, 1.0f);
    }

    Color::Color()
    {
        rgb.R   = 0.0f;
        rgb.G   = 0.0f;
        rgb.B   = 0.0f;
        hsl.H   = 0.0f;
        hsl.S   = 0.0f;
        hsl.L   = 0.0f;
        xyz.X   = 0.0f;
        xyz.Y   = 0.0f;
        xyz.Z   = 0.0f;
        mask    = M_RGB;
        A       = 0.0f;
    }

    Color::Color(float r, float g, float b)
    {
        set_rgb(r, g, b);
        hsl.H   = 0.0f;
        hsl.S   = 0.0f;
        hsl.L   = 0.0f;
        xyz.X   = 0.0f;
        xyz.Y   = 0.0f;
        xyz.Z   = 0.0f;
        A       = 0.0f;
    }

    Color::Color(float r, float g, float b, float a)
    {
        set_rgba(r, g, b, a);
        hsl.H   = 0.0f;
        hsl.S   = 0.0f;
        hsl.L   = 0.0f;
        xyz.X   = 0.0f;
        xyz.Y   = 0.0f;
        xyz.Z   = 0.0f;
    }

    Color::Color(const Color &src)
    {
        rgb     = src.rgb;
        hsl     = src.hsl;
        xyz     = src.xyz;
        mask    = src.mask;
        A       = src.A;
    }

    Color::Color(const Color &src, float a)
    {
        rgb     = src.rgb;
        hsl     = src.hsl;
        xyz     = src.xyz;
        mask    = src.mask;
        A       = clamp(a);
    }

    Color::Color(const Color *src)
    {
        rgb     = src->rgb;
        hsl     = src->hsl;
        xyz     = src->xyz;
        mask    = src->mask;
        A       = src->A;
    }

    Color::Color(const Color *src, float a)
    {
        rgb     = src->rgb;
        hsl     = src->hsl;
        xyz     = src->xyz;
        mask    = src->mask;
        A       = clamp(a);
    }

    Color::Color(uint32_t rgb)
    {
        set_rgb24(rgb);
        hsl.H   = 0.0f;
        hsl.S   = 0.0f;
        hsl.L   = 0.0f;
    }

    Color::Color(uint32_t rgb, float a)
    {
        set_rgb24(rgb);
        hsl.H   = 0.0f;
        hsl.S   = 0.0f;
        hsl.L   = 0.0f;
        A       = a;
    }

    Color &Color::red(float r)
    {
        calc_rgb().R    = clamp(r);
        mask            = M_RGB;
        return *this;
    }

    Color &Color::green(float g)
    {
        calc_rgb().G    = clamp(g);
        mask            = M_RGB;
        return *this;
    }

    Color &Color::blue(float b)
    {
        calc_rgb().B    = clamp(b);
        mask            = M_RGB;
        return *this;
    }

    Color &Color::hue(float h)
    {
        calc_hsl().H    = clamp(h);
        mask            = M_HSL;
        return *this;
    }

    Color &Color::saturation(float s)
    {
        calc_hsl().S    = clamp(s);
        mask            = M_HSL;
        return *this;
    }

    Color &Color::lightness(float l)
    {
        calc_hsl().L    = clamp(l);
        mask            = M_HSL;
        return *this;
    }

    Color &Color::hsl_hue(float h)
    {
        calc_hsl().H    = clamp(h);
        mask            = M_HSL;
        return *this;
    }

    Color &Color::hsl_saturation(float s)
    {
        calc_hsl().S    = clamp(s);
        mask            = M_HSL;
        return *this;
    }

    Color &Color::hsl_lightness(float l)
    {
        calc_hsl().L    = clamp(l);
        mask            = M_HSL;
        return *this;
    }

    Color &Color::alpha(float a)
    {
        A = a;
        return *this;
    }

    Color &Color::set_rgb24(uint32_t v)
    {
        rgb.R   = ((v >> 16) & 0xff) / 255.0f;
        rgb.G   = ((v >> 8) & 0xff) / 255.0f;
        rgb.B   = (v & 0xff) / 255.0f;
        A       = 0.0f;
        mask    = M_RGB;

        return *this;
    }

    Color &Color::set_rgba32(uint32_t v)
    {
        rgb.R   = ((v >> 16) & 0xff) / 255.0f;
        rgb.G   = ((v >> 8) & 0xff) / 255.0f;
        rgb.B   = (v & 0xff) / 255.0f;
        A       = ((v >> 24) & 0xff) / 255.0f;
        mask    = M_RGB;

        return *this;
    }

    Color &Color::set_hsl24(uint32_t v)
    {
        hsl.H   = ((v >> 16) & 0xff) / 255.0f;
        hsl.S   = ((v >> 8) & 0xff) / 255.0f;
        hsl.L   = (v & 0xff) / 255.0f;
        A       = 0.0f;
        mask    = M_HSL;

        return *this;
    }

    Color &Color::set_hsla32(uint32_t v)
    {
        hsl.H   = ((v >> 16) & 0xff) / 255.0f;
        hsl.S   = ((v >> 8) & 0xff) / 255.0f;
        hsl.L   = (v & 0xff) / 255.0f;
        A       = ((v >> 24) & 0xff) / 255.0f;
        mask    = M_HSL;

        return *this;
    }

    Color::rgb_t &Color::calc_rgb() const
    {
        if (mask & M_RGB)
            return rgb;

        if (mask & M_HSL)
        {
            // Convert HSL to RGB
            if (hsl.S > 0.0f)
            {
                float Q     = (hsl.L < 0.5f) ? hsl.L * (1.0f + hsl.S) : hsl.L + hsl.S - hsl.L*hsl.S;
                float P     = hsl.L + hsl.L - Q; // 2.0 * L - Q
                float D     = 6.0f * (Q - P);

                float TR    = hsl.H + HSL_RGB_1_3;
                float TG    = hsl.H;
                float TB    = hsl.H - HSL_RGB_1_3;

                if (TR > 1.0f)
                    TR  -= 1.0f;
                if (TB < 0.0f)
                    TB  += 1.0f;

                if (TR < 0.5f)
                    rgb.R       = (TR < HSL_RGB_1_6) ? P + D * TR : Q;
                else
                    rgb.R       = (TR < HSL_RGB_2_3) ? P + D * (HSL_RGB_2_3 -  TR) : P;

                if (TG < 0.5f)
                    rgb.G       = (TG < HSL_RGB_1_6) ? P + D * TG : Q;
                else
                    rgb.G       = (TG < HSL_RGB_2_3) ? P + D * (HSL_RGB_2_3 -  TG) : P;

                if (TB < 0.5f)
                    rgb.B       = (TB < HSL_RGB_1_6) ? P + D * TB : Q;
                else
                    rgb.B       = (TB < HSL_RGB_2_3) ? P + D * (HSL_RGB_2_3 -  TB) : P;
            }
            else
            {
                rgb.R   = hsl.L;
                rgb.G   = hsl.L;
                rgb.B   = hsl.L;
            }
        }
        else // mask & M_XYZ
        {
            // Convert XYZ to RGB
            float r = 0.01f * (xyz.X *  3.2406f + xyz.Y * -1.5372f + xyz.Z * -0.4986f);
            float g = 0.01f * (xyz.X * -0.9689f + xyz.Y *  1.8758f + xyz.Z *  0.0415f);
            float b = 0.01f * (xyz.X *  0.0557f + xyz.Y * -0.2040f + xyz.Z *  1.0570f);

            r       = (r > 0.0031308f) ? 1.055f * (powf(r, 1.0f / 2.4f)) - 0.055f : 12.92f * r;
            g       = (g > 0.0031308f) ? 1.055f * (powf(g, 1.0f / 2.4f)) - 0.055f : 12.92f * g;
            b       = (b > 0.0031308f) ? 1.055f * (powf(b, 1.0f / 2.4f)) - 0.055f : 12.92f * b;

            rgb.R   = clamp(r);
            rgb.G   = clamp(g);
            rgb.B   = clamp(b);
        } // else

        mask |= M_RGB;
        return rgb;
    }

    Color::hsl_t &Color::calc_hsl() const
    {
        if (mask & M_HSL)
            return hsl;

        // At this moment we can convert color to HSL only from RGB
        calc_rgb();

        float cmax = (rgb.R < rgb.G) ? ((rgb.B < rgb.G) ? rgb.G : rgb.B) : ((rgb.B < rgb.R) ? rgb.R : rgb.B);
        float cmin = (rgb.R < rgb.G) ? ((rgb.B < rgb.R) ? rgb.B : rgb.R) : ((rgb.B < rgb.G) ? rgb.B : rgb.G);
        float d = cmax - cmin;

        hsl.H   = 0.0f;
        hsl.S   = 0.0f;
        hsl.L   = 0.5f * (cmax + cmin);

        // Calculate hue
        if (rgb.R == cmax)
        {
            hsl.H = (rgb.G - rgb.B) / d;
            if (rgb.G < rgb.B)
                hsl.H += 6.0f;
        }
        else if (rgb.G == cmax)
            hsl.H = (rgb.B - rgb.R) / d + 2.0f;
        else if (rgb.B == cmax)
            hsl.H = (rgb.R - rgb.G) / d + 4.0f;

        // Calculate saturation
        if (hsl.L <= 0.5f)
            hsl.S = (hsl.L <= 0.0f) ? 0.0f : d / hsl.L;
        else if (hsl.L > 0.5f)
            hsl.S = (hsl.L < 1.0f) ? d / (1.0f - hsl.L) : 0.0f;

        // Normalize hue
        hsl.H  /= 6.0f;
        hsl.S  *= 0.5f;

        mask |= M_HSL;

        return hsl;
    }

    Color::xyz_t &Color::calc_xyz() const
    {
        if (mask & M_XYZ)
            return xyz;

        // At this moment we can convert color to XYZ only from RGB
        calc_rgb();

        float r     = (rgb.R > 0.04045f) ? powf((rgb.R + 0.055f) / 1.055f, 2.4f) : rgb.R / 12.92f;
        float g     = (rgb.G > 0.04045f) ? powf((rgb.G + 0.055f) / 1.055f, 2.4f) : rgb.G / 12.92f;
        float b     = (rgb.B > 0.04045f) ? powf((rgb.B + 0.055f) / 1.055f, 2.4f) : rgb.B / 12.92f;

        xyz.X       = 100.0f * (r * 0.4124f + g * 0.3576f + b * 0.1805f);
        xyz.Y       = 100.0f * (r * 0.2126f + g * 0.7152f + b * 0.0722f);
        xyz.Z       = 100.0f * (r * 0.0193f + g * 0.1192f + b * 0.9505f);

        return xyz;
    }

    Color &Color::set_rgb(float r, float g, float b)
    {
        mask    = M_RGB;
        rgb.R   = clamp(r);
        rgb.G   = clamp(g);
        rgb.B   = clamp(b);

        return *this;
    }

    Color &Color::set_rgba(float r, float g, float b, float a)
    {
        mask    = M_RGB;
        rgb.R   = clamp(r);
        rgb.G   = clamp(g);
        rgb.B   = clamp(b);
        A       = clamp(a);

        return *this;
    }

    Color &Color::set_hsl(float h, float s, float l)
    {
        mask    = M_HSL;
        hsl.H   = clamp(h);
        hsl.S   = clamp(s);
        hsl.L   = clamp(l);

        return *this;
    }

    Color &Color::set_hsla(float h, float s, float l, float a)
    {
        mask    = M_HSL;
        hsl.H   = clamp(h);
        hsl.S   = clamp(s);
        hsl.L   = clamp(l);
        A       = clamp(a);

        return *this;
    }

    const Color &Color::get_rgb(float &r, float &g, float &b) const
    {
        calc_rgb();
        r       = rgb.R;
        g       = rgb.G;
        b       = rgb.B;

        return *this;
    }

    const Color &Color::get_rgba(float &r, float &g, float &b, float &a) const
    {
        calc_rgb();
        r       = rgb.R;
        g       = rgb.G;
        b       = rgb.B;
        a       = A;

        return *this;
    }

    const Color &Color::get_hsl(float &h, float &s, float &l) const
    {
        calc_hsl();
        h       = hsl.H;
        s       = hsl.S;
        l       = hsl.L;

        return *this;
    }

    const Color &Color::get_hsla(float &h, float &s, float &l, float &a) const
    {
        calc_hsl();
        h       = hsl.H;
        s       = hsl.S;
        l       = hsl.L;
        a       = A;

        return *this;
    }

    Color &Color::get_rgb(float &r, float &g, float &b)
    {
        calc_rgb();
        r       = rgb.R;
        g       = rgb.G;
        b       = rgb.B;

        return *this;
    }

    Color &Color::get_rgba(float &r, float &g, float &b, float &a)
    {
        calc_rgb();
        r       = rgb.R;
        g       = rgb.G;
        b       = rgb.B;
        a       = A;

        return *this;
    }

    Color &Color::get_hsl(float &h, float &s, float &l)
    {
        calc_hsl();
        h       = hsl.H;
        s       = hsl.S;
        l       = hsl.L;

        return *this;
    }

    Color &Color::get_hsla(float &h, float &s, float &l, float &a)
    {
        calc_hsl();
        h       = hsl.H;
        s       = hsl.S;
        l       = hsl.L;
        a       = A;

        return *this;
    }


    Color &Color::blend(const Color &c, float alpha)
    {
        float r1, g1, b1, r2, g2, b2;
        get_rgb(r1, g1, b1);
        c.get_rgb(r2, g2, b2);
        set_rgb(r2 + (r1 - r2) * alpha, g2 + (g1 - g2) * alpha, b2 + (b1 - b2) * alpha);

        return *this;
    }

    Color &Color::blend(float r, float g, float b, float alpha)
    {
        float r1, g1, b1;
        get_rgb(r1, g1, b1);
        set_rgb(r + (r1 - r) * alpha, g + (g1 - g) * alpha, b + (b1 - b) * alpha);

        return *this;
    }

    Color &Color::blend(const Color &c1, const Color &c2, float alpha)
    {
        float r1, g1, b1, r2, g2, b2;
        c1.get_rgb(r1, g1, b1);
        c2.get_rgb(r2, g2, b2);
        set_rgb(r2 + (r1 - r2) * alpha, g2 + (g1 - g2) * alpha, b2 + (b1 - b2) * alpha);

        return *this;
    }

    Color &Color::darken(float amount)
    {
        float r, g, b;
        get_rgb(r, g, b);

        float a = 1.0 - amount;
        set_rgb(r * a, g * a, b * a);

        return *this;
    }

    Color &Color::lighten(float amount)
    {
        float r, g, b;
        get_rgb(r, g, b);

        float a = 1.0 - amount;
        set_rgb(r + (1.0 - r) * a, g + (1.0 - g) * a, b + (1.0 - b) * a);

        return *this;
    }

    void Color::scale_lightness(float amount)
    {
        calc_hsl().L    = clamp(amount * hsl.L);
        mask    = M_HSL;
    }

    Color &Color::copy(const Color &c)
    {
        rgb     = c.rgb;
        hsl     = c.hsl;
        xyz     = c.xyz;
        A       = c.A;
        mask    = c.mask;

        return *this;
    }

    Color &Color::copy(const Color *c)
    {
        rgb     = c->rgb;
        hsl     = c->hsl;
        xyz     = c->xyz;
        A       = c->A;
        mask    = c->mask;

        return *this;
    }

    Color &Color::copy(const Color &c, float a)
    {
        rgb     = c.rgb;
        hsl     = c.hsl;
        xyz     = c.xyz;
        A       = clamp(a);
        mask    = c.mask;

        return *this;
    }

    Color &Color::copy(const Color *c, float a)
    {
        rgb     = c->rgb;
        hsl     = c->hsl;
        xyz     = c->xyz;
        A       = clamp(a);
        mask    = c->mask;

        return *this;
    }

    ssize_t Color::format(char *dst, size_t len, size_t tolerance, const float *v, char prefix, bool alpha)
    {
        if ((dst == NULL) || (tolerance <= 0) || (tolerance > 4))
            return -STATUS_BAD_ARGUMENTS;
        size_t required = (tolerance * (alpha ? 4 : 3)) + 2; // Number of hex characters x number of colors + 2 symbols
        if (len < required)
            return -STATUS_OVERFLOW;

        // Calculate maximum value
        const char *fmt;
        int tol;

        if (alpha)
        {
            switch (tolerance)
            {
                case 1:
                    fmt = "%c%01x%01x%01x%01x";
                    tol = 0xf;
                    break;
                case 3:
                    fmt = "%c%03x%03x%03x%03x";
                    tol = 0xfff;
                    break;
                case 4:
                    fmt = "%c%04x%04x%04x%04x";
                    tol = 0xffff;
                    break;
                default:
                    fmt = "%c%02x%02x%02x%02x";
                    tol = 0xff;
                    break;
            }

            return ::snprintf(
                    dst, len, fmt, prefix,
                    int(v[3] * tol + 0.25f) & tol,
                    int(v[0] * tol + 0.25f) & tol,
                    int(v[1] * tol + 0.25f) & tol,
                    int(v[2] * tol + 0.25f) & tol
                );
        }
        else
        {
            switch (tolerance)
            {
                case 1:
                    fmt = "%c%01x%01x%01x";
                    tol = 0xf;
                    break;
                case 3:
                    fmt = "%c%03x%03x%03x";
                    tol = 0xfff;
                    break;
                case 4:
                    fmt = "%c%04x%04x%04x";
                    tol = 0xffff;
                    break;
                default:
                    fmt = "%c%02x%02x%02x";
                    tol = 0xff;
                    break;
            }

            return ::snprintf(
                    dst, len, fmt, prefix,
                    int(v[0] * tol + 0.25f) & tol,
                    int(v[1] * tol + 0.25f) & tol,
                    int(v[2] * tol + 0.25f) & tol
                );
        }
    }

    ssize_t Color::format_rgb(char *dst, size_t len, size_t tolerance) const
    {
        float v[3];
        get_rgb(v[0], v[1], v[2]);
        return format(dst, len, tolerance, v, '#', false);
    }

    ssize_t Color::format_hsl(char *dst, size_t len, size_t tolerance) const
    {
        float v[3];
        get_hsl(v[0], v[1], v[2]);
        return format(dst, len, tolerance, v, '@', false);
    }

    ssize_t Color::format_rgba(char *dst, size_t len, size_t tolerance) const
    {
        float v[4];
        get_rgba(v[0], v[1], v[2], v[3]);
        return format(dst, len, tolerance, v, '#', true);
    }

    ssize_t Color::format_hsla(char *dst, size_t len, size_t tolerance) const
    {
        float v[4];
        get_hsla(v[0], v[1], v[2], v[3]);
        return format(dst, len, tolerance, v, '@', true);
    }

    status_t Color::parse_hex(float *dst, size_t n, char prefix, const char *src, size_t len)
    {
        if (src == NULL)
            return STATUS_BAD_ARGUMENTS;

        const char *end = &src[len];

        src = skip_whitespace(src, end);
        if (src == end)
            return STATUS_NO_DATA;
        if (*(src++) != prefix)
            return STATUS_BAD_FORMAT;
        if (src >= end)
            return STATUS_BAD_FORMAT;

        const char *hex  = skip_hex(src, end);
        const char *tail = skip_whitespace(hex, end);
        if (tail != end)
            return STATUS_BAD_FORMAT;
        size_t hex_len   = hex - src;

        // Determine the length of each component
        if (hex_len % n)
            return STATUS_BAD_FORMAT;
        hex_len /= n;
        if ((hex_len <= 0) || (hex_len > 4))
            return STATUS_BAD_FORMAT;
        float norm = 1.0f / ((0x1 << (hex_len << 2)) - 1);

        // Read components
        while (n--)
        {
            int v       = 0;
            for (size_t i=0; i<hex_len; ++i, ++src)
                v   = (v << 4) | hex_char(*src);

            *(dst++)    = v * norm;
        }

        return STATUS_OK;
    }

    status_t Color::parse_numeric(float *dst, size_t nmin, size_t nmax, const char *prefix, const char *src, size_t len)
    {
        // Save and update locale
        char *saved = ::setlocale(LC_NUMERIC, NULL);
        if (saved != NULL)
        {
            size_t len = ::strlen(saved) + 1;
            char *saved_copy = static_cast<char *>(alloca(len));
            ::memcpy(saved_copy, saved, len);
            saved       = saved_copy;
        }
        ::setlocale(LC_NUMERIC, "C");

        status_t res = parse_cnumeric(dst, nmin, nmax, prefix, src, len);

        if (saved != NULL)
            ::setlocale(LC_NUMERIC, saved);

        return res;
    }

    status_t Color::parse_cnumeric(float *dst, size_t nmin, size_t nmax, const char *prefix, const char *src, size_t len)
    {
        const char *end = &src[len];

        // Parse beginning of statement
        if ((src = skip_whitespace(src, end)) == end)       // whitespace
            return STATUS_NO_DATA;
        if ((src = match_prefix(src, end, prefix)) == end)  // prefix
            return STATUS_BAD_FORMAT;
        if ((src = skip_whitespace(src, end)) == end)       // whitespace
            return STATUS_BAD_FORMAT;
        if ((src = match_char(src, end, '(')) == end)       // (
            return STATUS_BAD_FORMAT;

        // Main loop: parse digits
        size_t count = 0;
        while (src < end)
        {
            // Skip whitespace
            if ((src = skip_whitespace(src, end)) == end)
                return STATUS_BAD_FORMAT;

            // Find next delimiting character
            const char *next = match_chars(src, end, ",) \t\r\n");
            if (next == end)
                return STATUS_BAD_FORMAT;

            // Overflow of number of max possible values?
            if (count >= nmax)
                return STATUS_BAD_FORMAT;

            // Parse floating-point value
            char *eptr = NULL;
            errno = 0;
            dst[count++] = strtof(src, &eptr);
            if ((errno != 0) || (eptr != next) || (eptr == src))
                return STATUS_BAD_FORMAT;

            // Move parse position
            src     = skip_whitespace(next, end);
            if (src == end)
                return STATUS_BAD_FORMAT;
            char c = *(src++);
            if (c == ')')
                break;
            else if (c != ',')
                return STATUS_BAD_FORMAT;
        }

        // Check that we parsed at least nmin floating-point values
        if (count < nmin)
            return STATUS_BAD_FORMAT;

        // Parse end of statement
        if ((src = skip_whitespace(src, end)) != end)       // end-of-line
            return STATUS_BAD_FORMAT;

        return STATUS_OK;
    }

    status_t Color::parse4(const char *src, size_t len)
    {
        if (src == NULL)
            return STATUS_BAD_ARGUMENTS;

        const char *end = &src[len];
        src = skip_whitespace(src, end);
        if (src >= end)
            return STATUS_NO_DATA;

        return (*src == '@') ? parse_hsla(src, end - src) : parse_rgba(src, end - src);
    }

    status_t Color::parse3(const char *src, size_t len)
    {
        if (src == NULL)
            return STATUS_BAD_ARGUMENTS;

        const char *end = &src[len];
        src = skip_whitespace(src, end);
        if (src >= end)
            return STATUS_NO_DATA;

        return (*src == '@') ? parse_hsl(src, end - src) : parse_rgb(src, end - src);
    }

    status_t Color::parse_rgba(const char *src, size_t len)
    {
        float v[4];
        status_t res = parse_hex(v, 4, '#', src, len);
        if (res == STATUS_OK)
            set_rgba(v[1], v[2], v[3], v[0]);
        return res;
    }

    status_t Color::parse_hsla(const char *src, size_t len)
    {
        float v[4];
        status_t res = parse_hex(v, 4, '@', src, len);
        if (res == STATUS_OK)
            set_hsla(v[1], v[2], v[3], v[0]);
        return res;
    }

    status_t Color::parse_rgb(const char *src, size_t len)
    {
        float v[3];
        status_t res = parse_hex(v, 3, '#', src, len);
        if (res == STATUS_OK)
            set_rgba(v[0], v[1], v[2], 0.0f);
        return res;
    }

    status_t Color::parse_hsl(const char *src, size_t len)
    {
        float v[3];
        status_t res = parse_hex(v, 3, '@', src, len);
        if (res == STATUS_OK)
            set_hsla(v[0], v[1], v[2], 0.0f);
        return res;
    }

    status_t Color::parse(const char *src, size_t len)
    {
        status_t res;
        float v[4];
        if ((res = parse4(src, len)) == STATUS_OK)
            return res;
        if ((res = parse3(src, len)) == STATUS_OK)
            return res;

        // Save and update locale
        char *saved = ::setlocale(LC_NUMERIC, NULL);
        if (saved != NULL)
        {
            size_t len = ::strlen(saved) + 1;
            char *saved_copy = static_cast<char *>(alloca(len));
            ::memcpy(saved_copy, saved, len);
            saved       = saved_copy;
        }
        ::setlocale(LC_NUMERIC, "C");

        // Try to parse different expressions
        if ((res = parse_cnumeric(v, 3, 3, "rgb", src, len)) == STATUS_OK)
            set_rgba(v[0], v[1], v[2], 0.0f);
        else if ((res = parse_cnumeric(v, 4, 4, "rgba", src, len)) == STATUS_OK)
            set_rgba(v[0], v[1], v[2], v[3]);
        else if ((res = parse_cnumeric(v, 3, 3, "hsl", src, len)) == STATUS_OK)
            set_hsla(v[0] / 360.0f, v[1] * 0.01f, v[2] * 0.005f, 0.0f);
        else if ((res = parse_cnumeric(v, 4, 4, "hsla", src, len)) == STATUS_OK)
            set_hsla(v[0] / 360.0f, v[1] * 0.01f, v[2] * 0.005f, v[3]);
        else if ((res = parse_cnumeric(v, 3, 3, "xyz", src, len)) == STATUS_OK)
            set_xyza(v[0], v[1], v[2], 0.0f);
        else if ((res = parse_cnumeric(v, 4, 4, "xyza", src, len)) == STATUS_OK)
            set_xyza(v[0], v[1], v[2], v[3]);

        if (saved != NULL)
            ::setlocale(LC_NUMERIC, saved);

        return res;
    }

    status_t Color::parse_rgba(const char *src)
    {
        return (src != NULL) ? parse_rgba(src, ::strlen(src)) : STATUS_BAD_ARGUMENTS;
    }

    status_t Color::parse_hsla(const char *src)
    {
        return (src != NULL) ? parse_hsla(src, ::strlen(src)) : STATUS_BAD_ARGUMENTS;
    }

    status_t Color::parse_rgb(const char *src)
    {
        return (src != NULL) ? parse_rgb(src, ::strlen(src)) : STATUS_BAD_ARGUMENTS;
    }

    status_t Color::parse_hsl(const char *src)
    {
        return (src != NULL) ? parse_hsl(src, ::strlen(src)) : STATUS_BAD_ARGUMENTS;
    }

    status_t Color::parse4(const char *src)
    {
        return (src != NULL) ? parse4(src, ::strlen(src)) : STATUS_BAD_ARGUMENTS;
    }

    status_t Color::parse3(const char *src)
    {
        return (src != NULL) ? parse3(src, ::strlen(src)) : STATUS_BAD_ARGUMENTS;
    }

    status_t Color::parse(const char *src)
    {
        return (src != NULL) ? parse(src, ::strlen(src)) : STATUS_BAD_ARGUMENTS;
    }

    uint32_t Color::rgb24() const
    {
        calc_rgb();
        return
            (uint32_t(rgb.R * 0xff + 0.25f) << 16) |
            (uint32_t(rgb.G * 0xff + 0.25f) << 8) |
            (uint32_t(rgb.B * 0xff + 0.25f) << 0);
    }

    uint32_t Color::rgba32() const
    {
        calc_rgb();
        return
            (uint32_t(A * 0xff + 0.25f) << 24) |
            (uint32_t(rgb.R * 0xff + 0.25f) << 16) |
            (uint32_t(rgb.G * 0xff + 0.25f) << 8) |
            (uint32_t(rgb.B * 0xff + 0.25f) << 0);
    }

    uint32_t Color::hsl24() const
    {
        calc_hsl();
        return
            (uint32_t(hsl.H * 0xff + 0.25f) << 16) |
            (uint32_t(hsl.S * 0xff + 0.25f) << 8) |
            (uint32_t(hsl.L * 0xff + 0.25f) << 0);
    }

    uint32_t Color::hsla32() const
    {
        calc_hsl();
        return
            (uint32_t(A * 0xff + 0.25f) << 24) |
            (uint32_t(hsl.H * 0xff + 0.25f) << 16) |
            (uint32_t(hsl.S * 0xff + 0.25f) << 8) |
            (uint32_t(hsl.L * 0xff + 0.25f) << 0);
    }

    void Color::swap(Color *c)
    {
        lsp::swap(rgb, c->rgb);
        lsp::swap(hsl, c->hsl);
        lsp::swap(A, c->A);
        lsp::swap(mask, c->mask);
    }

    const Color &Color::get_xyz(float &x, float &y, float &z) const
    {
        calc_xyz();
        x   = xyz.X;
        y   = xyz.Y;
        z   = xyz.Z;

        return *this;
    }

    Color &Color::get_xyz(float &x, float &y, float &z)
    {
        calc_xyz();
        x   = xyz.X;
        y   = xyz.Y;
        z   = xyz.Z;

        return *this;
    }

    const Color &Color::get_xyza(float &x, float &y, float &z, float &a) const
    {
        calc_xyz();
        x   = xyz.X;
        y   = xyz.Y;
        z   = xyz.Z;
        a   = A;

        return *this;
    }

    Color &Color::get_xyza(float &x, float &y, float &z, float &a)
    {
        calc_xyz();
        x   = xyz.X;
        y   = xyz.Y;
        z   = xyz.Z;
        a   = A;

        return *this;
    }

    Color &Color::xyz_x(float x)
    {
        calc_xyz().X    = x;
        mask    = M_XYZ;
        return *this;
    }

    Color &Color::xyz_y(float y)
    {
        calc_xyz().Y    = y;
        mask    = M_XYZ;
        return *this;
    }

    Color &Color::xyz_z(float z)
    {
        calc_xyz().Z    = z;
        mask    = M_XYZ;
        return *this;
    }

    Color &Color::set_xyz(float x, float y, float z)
    {
        xyz.X   = lsp_limit(x, 0.0f, 100.0f);
        xyz.Y   = lsp_limit(y, 0.0f, 100.0f);
        xyz.Z   = lsp_limit(z, 0.0f, 110.0f);
        mask    = M_XYZ;
        return *this;
    }

    Color &Color::set_xyza(float x, float y, float z, float a)
    {
        xyz.X   = lsp_limit(x, 0.0f, 100.0f);
        xyz.Y   = lsp_limit(y, 0.0f, 100.0f);
        xyz.Z   = lsp_limit(z, 0.0f, 110.0f);
        A       = a;
        mask    = M_XYZ;
        return *this;
    }

} /* namespace lsp */
