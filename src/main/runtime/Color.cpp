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

    Color::Color(uint32_t rgb)
    {
        H       = 0.0f;
        S       = 0.0f;
        L       = 0.0f;
        set_rgb24(rgb);
    }

    Color::Color(uint32_t rgb, float a)
    {
        H       = 0.0f;
        S       = 0.0f;
        L       = 0.0f;
        set_rgb24(rgb);
        A       = a;
    }

    void Color::set_rgb24(uint32_t v)
    {
        R       = ((v >> 16) & 0xff) / 255.0f;
        G       = ((v >> 8) & 0xff) / 255.0f;
        B       = (v & 0xff) / 255.0f;
        A       = 0.0f;
        nMask   = M_RGB;
    }

    void Color::set_rgba32(uint32_t v)
    {
        R       = ((v >> 16) & 0xff) / 255.0f;
        G       = ((v >> 8) & 0xff) / 255.0f;
        B       = (v & 0xff) / 255.0f;
        A       = ((v >> 24) & 0xff) / 255.0f;
        nMask   = M_RGB;
    }

    void Color::set_hsl24(uint32_t v)
    {
        H       = ((v >> 16) & 0xff) / 255.0f;
        S       = ((v >> 8) & 0xff) / 255.0f;
        L       = (v & 0xff) / 255.0f;
        A       = 0.0f;
        nMask   = M_HSL;
    }

    void Color::set_hsla32(uint32_t v)
    {
        H       = ((v >> 16) & 0xff) / 255.0f;
        S       = ((v >> 8) & 0xff) / 255.0f;
        L       = (v & 0xff) / 255.0f;
        A       = ((v >> 24) & 0xff) / 255.0f;
        nMask   = M_HSL;
    }

    void Color::calc_rgb() const
    {
        if (nMask & M_RGB)
            return;

        if (S > 0.0f)
        {
            float Q     = (L < 0.5f) ? L * (1.0f + S) : L + S - L*S;
            float P     = L + L - Q; // 2.0 * L - Q
            float D     = 6.0f * (Q - P);

            float TR    = H + HSL_RGB_1_3;
            float TG    = H;
            float TB    = H - HSL_RGB_1_3;

            if (TR > 1.0f)
                TR  -= 1.0f;
            if (TB < 0.0f)
                TB  += 1.0f;

            if (TR < 0.5f)
                R       = (TR < HSL_RGB_1_6) ? P + D * TR : Q;
            else
                R       = (TR < HSL_RGB_2_3) ? P + D * (HSL_RGB_2_3 -  TR) : P;

            if (TG < 0.5f)
                G       = (TG < HSL_RGB_1_6) ? P + D * TG : Q;
            else
                G       = (TG < HSL_RGB_2_3) ? P + D * (HSL_RGB_2_3 -  TG) : P;

            if (TB < 0.5f)
                B       = (TB < HSL_RGB_1_6) ? P + D * TB : Q;
            else
                B       = (TB < HSL_RGB_2_3) ? P + D * (HSL_RGB_2_3 -  TB) : P;
        }
        else
        {
            R = L;
            G = L;
            B = L;
        }

        nMask |= M_RGB;
    }

    void Color::calc_hsl() const
    {
        if (nMask & M_HSL)
            return;

        float cmax = (R < G) ? ((B < G) ? G : B) : ((B < R) ? R : B);
        float cmin = (R < G) ? ((B < R) ? B : R) : ((B < G) ? B : G);
        float d = cmax - cmin;

        H = 0.0f;
        S = 0.0f;
        L = 0.5f * (cmax + cmin);

        // Calculate hue
        if (R == cmax)
        {
            H = (G - B) / d;
            if (G < B)
                H += 6.0f;
        }
        else if (G == cmax)
            H = (B - R) / d + 2.0f;
        else if (B == cmax)
            H = (R - G) / d + 4.0f;

        // Calculate saturation
        if (L <= 0.5f)
            S = (L <= 0.0f) ? 0.0f : d / L;
        else if (L > 0.5f)
            S = d / (1.0f - L);

        // Normalize hue
        H  /= 6.0f;
        S  *= 0.5f;

        nMask |= M_HSL;
    }

    void Color::check_rgb() const
    {
        if (nMask & M_RGB)
            return;

        calc_rgb();
        nMask |= M_RGB;
    };

    void Color::check_hsl() const
    {
        if (nMask & M_HSL)
            return;

        calc_hsl();
        nMask |= M_HSL;
    };

    void Color::blend(const Color &c, float alpha)
    {
        float r1, g1, b1, r2, g2, b2;
        get_rgb(r1, g1, b1);
        c.get_rgb(r2, g2, b2);
        set_rgb(r2 + (r1 - r2) * alpha, g2 + (g1 - g2) * alpha, b2 + (b1 - b2) * alpha);
    }

    void Color::blend(float r, float g, float b, float alpha)
    {
        float r1, g1, b1;
        get_rgb(r1, g1, b1);
        set_rgb(r + (r1 - r) * alpha, g + (g1 - g) * alpha, b + (b1 - b) * alpha);
    }

    void Color::blend(const Color &c1, const Color &c2, float alpha)
    {
        float r1, g1, b1, r2, g2, b2;
        c1.get_rgb(r1, g1, b1);
        c2.get_rgb(r2, g2, b2);
        set_rgb(r2 + (r1 - r2) * alpha, g2 + (g1 - g2) * alpha, b2 + (b1 - b2) * alpha);
    }

    void Color::darken(float amount)
    {
        float r, g, b;
        get_rgb(r, g, b);

        float a = 1.0 - amount;
        set_rgb(r * a, g * a, b * a);
    }

    void Color::lighten(float amount)
    {
        float r, g, b;
        get_rgb(r, g, b);

        float a = 1.0 - amount;
        set_rgb(r + (1.0 - r) * a, g + (1.0 - g) * a, b + (1.0 - b) * a);
    }

    void Color::scale_lightness(float amount)
    {
        check_hsl();
        L *= amount;
        if (L < 0.0f)
            L = 0.0f;
        else if (L > 1.0f)
            L = 1.0f;
        nMask = M_HSL;
    }

    void Color::copy(const Color &c)
    {
        R       = c.R;
        G       = c.G;
        B       = c.B;
        H       = c.H;
        S       = c.S;
        L       = c.L;
        A       = c.A;
        nMask   = c.nMask & (M_RGB | M_HSL);
    }

    void Color::copy(const Color *c)
    {
        R       = c->R;
        G       = c->G;
        B       = c->B;
        H       = c->H;
        S       = c->S;
        L       = c->L;
        A       = c->A;
        nMask   = c->nMask & (M_RGB | M_HSL);
    }

    void Color::copy(const Color &c, float a)
    {
        R       = c.R;
        G       = c.G;
        B       = c.B;
        H       = c.H;
        S       = c.S;
        L       = c.L;
        A       = a;
        nMask   = c.nMask & (M_RGB | M_HSL);
    }

    void Color::copy(const Color *c, float a)
    {
        R       = c->R;
        G       = c->G;
        B       = c->B;
        H       = c->H;
        S       = c->S;
        L       = c->L;
        A       = a;
        nMask   = c->nMask & (M_RGB | M_HSL);
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
        check_rgb();
        return
            (uint32_t(R * 0xff + 0.25f) << 16) |
            (uint32_t(G * 0xff + 0.25f) << 8) |
            (uint32_t(B * 0xff + 0.25f) << 0);
    }

    uint32_t Color::rgba32() const
    {
        check_rgb();
        return
            (uint32_t(A * 0xff + 0.25f) << 24) |
            (uint32_t(R * 0xff + 0.25f) << 16) |
            (uint32_t(G * 0xff + 0.25f) << 8) |
            (uint32_t(B * 0xff + 0.25f) << 0);
    }

    uint32_t Color::hsl24() const
    {
        check_hsl();
        return
            (uint32_t(H * 0xff + 0.25f) << 16) |
            (uint32_t(S * 0xff + 0.25f) << 8) |
            (uint32_t(L * 0xff + 0.25f) << 0);
    }

    uint32_t Color::hsla32() const
    {
        check_rgb();
        return
            (uint32_t(A * 0xff + 0.25f) << 24) |
            (uint32_t(H * 0xff + 0.25f) << 16) |
            (uint32_t(S * 0xff + 0.25f) << 8) |
            (uint32_t(L * 0xff + 0.25f) << 0);
    }

    void Color::swap(Color *c)
    {
        lsp::swap(R, c->R);
        lsp::swap(G, c->G);
        lsp::swap(B, c->B);
        lsp::swap(H, c->H);
        lsp::swap(S, c->S);
        lsp::swap(L, c->L);
        lsp::swap(A, c->A);
        lsp::swap(nMask, c->nMask);
    }

} /* namespace lsp */
