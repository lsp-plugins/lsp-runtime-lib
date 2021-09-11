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

namespace lsp
{
    static const float HSL_RGB_1_3          = 1.0f / 3.0f;
    static const float HSL_RGB_1_6          = 1.0f / 6.0f;
    static const float HSL_RGB_2_3          = 2.0f / 3.0f;

    static size_t skip_space(const char *ptr, size_t off, size_t len)
    {
        for (; off < len; ++off)
        {
            char c = ptr[off];
            if (c == '\0')
                return len;

            switch (c)
            {
                case ' ':
                case '\n':
                case '\t':
                case '\r':
                    break;
                default:
                    return off;
            }
        }
        return off;
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

    static size_t skip_hex(const char *ptr, size_t off, size_t len)
    {
        for (; off < len; ++off)
        {
            char c = ptr[off];
            if (hex_char(c) >= 16)
                return off;
        }
        return off;
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
        mask    = M_RGB;
        A       = 0.0f;
    }

    Color::Color(float r, float g, float b)
    {
        set_rgb(r, g, b);
        hsl.H   = 0.0f;
        hsl.S   = 0.0f;
        hsl.L   = 0.0f;
        A       = 0.0f;
    }

    Color::Color(float r, float g, float b, float a)
    {
        set_rgba(r, g, b, a);
        hsl.H   = 0.0f;
        hsl.S   = 0.0f;
        hsl.L   = 0.0f;
    }

    Color::Color(const Color &src)
    {
        rgb     = src.rgb;
        hsl     = src.hsl;
        mask    = src.mask;
        A       = src.A;
    }

    Color::Color(const Color &src, float a)
    {
        rgb     = src.rgb;
        hsl     = src.hsl;
        mask    = src.mask;
        A       = clamp(a);
    }

    Color::Color(const Color *src)
    {
        rgb     = src->rgb;
        hsl     = src->hsl;
        mask    = src->mask;
        A       = src->A;
    }

    Color::Color(const Color *src, float a)
    {
        rgb     = src->rgb;
        hsl     = src->hsl;
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
        A       = c.A;
        mask    = c.mask;

        return *this;
    }

    Color &Color::copy(const Color *c)
    {
        rgb     = c->rgb;
        hsl     = c->hsl;
        A       = c->A;
        mask    = c->mask;

        return *this;
    }

    Color &Color::copy(const Color &c, float a)
    {
        rgb     = c.rgb;
        hsl     = c.hsl;
        A       = clamp(a);
        mask    = c.mask;

        return *this;
    }

    Color &Color::copy(const Color *c, float a)
    {
        rgb     = c->rgb;
        hsl     = c->hsl;
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

    status_t Color::parse(float *dst, size_t n, char prefix, const char *src, size_t len)
    {
        if (src == NULL)
            return STATUS_BAD_ARGUMENTS;

        size_t off = skip_space(src, 0, len);
        if (off >= len)
            return STATUS_NO_DATA;
        if (src[off] != prefix)
            return STATUS_BAD_FORMAT;
        if ((++off) >= len)
            return STATUS_BAD_FORMAT;

        size_t off2 = skip_hex(src, off, len);
        size_t off3 = skip_space(src, off2, len);
        if (off3 < len)
            return STATUS_BAD_FORMAT;
        len     = off2 - off;

        // Determine the length of each component
        if (len % n)
            return STATUS_BAD_FORMAT;
        len /= n;
        if ((len <= 0) || (len > 4))
            return STATUS_BAD_FORMAT;
        float norm = 1.0f / ((0x1 << (len << 2)) - 1);

        // Read components
        while (n--)
        {
            int v       = 0;
            for (size_t i=0; i<len; ++i, ++off)
                v   = (v << 4) | hex_char(src[off]);

            *(dst++)    = v * norm;
        }

        return STATUS_OK;
    }

    status_t Color::parse4(const char *src, size_t len)
    {
        if (src == NULL)
            return STATUS_BAD_ARGUMENTS;

        size_t off = skip_space(src, 0, len);
        if (off >= len)
            return STATUS_NO_DATA;

        return (src[off] == '@') ? parse_hsla(&src[off], len - off) : parse_rgba(&src[off], len - off);
    }

    status_t Color::parse3(const char *src, size_t len)
    {
        if (src == NULL)
            return STATUS_BAD_ARGUMENTS;

        size_t off = skip_space(src, 0, len);
        if (off >= len)
            return STATUS_NO_DATA;

        return (src[off] == '@') ? parse_hsl(&src[off], len - off) : parse_rgb(&src[off], len - off);
    }

    status_t Color::parse_rgba(const char *src, size_t len)
    {
        float v[4];
        status_t res = parse(v, 4, '#', src, len);
        if (res == STATUS_OK)
            set_rgba(v[1], v[2], v[3], v[0]);
        return res;
    }

    status_t Color::parse_hsla(const char *src, size_t len)
    {
        float v[4];
        status_t res = parse(v, 4, '@', src, len);
        if (res == STATUS_OK)
            set_hsla(v[1], v[2], v[3], v[0]);
        return res;
    }

    status_t Color::parse_rgb(const char *src, size_t len)
    {
        float v[3];
        status_t res = parse(v, 3, '#', src, len);
        if (res == STATUS_OK)
            set_rgba(v[0], v[1], v[2], 0.0f);
        return res;
    }

    status_t Color::parse_hsl(const char *src, size_t len)
    {
        float v[3];
        status_t res = parse(v, 3, '@', src, len);
        if (res == STATUS_OK)
            set_hsla(v[0], v[1], v[2], 0.0f);
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

} /* namespace lsp */
