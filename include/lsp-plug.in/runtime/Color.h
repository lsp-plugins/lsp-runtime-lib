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

#ifndef LSP_PLUG_IN_RUNTIME_COLOR_H_
#define LSP_PLUG_IN_RUNTIME_COLOR_H_

#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/runtime/LSPString.h>

namespace lsp
{
    /**
     * Color class. Represents an object that allows to work with different color models.
     * Supported color models:
     *   - RGB (red, green, blue components)
     *   - HSL (hue, saturation, lightness components)
     *   - Alpha channel
     * All components are normalzed within the [0.0f .. 1.0f] range
     */
    class Color
    {
        protected:
            enum mask_t
            {
                M_RGB           = 1 << 0,   // Standard RGB color components
                M_HSL           = 1 << 1,   // Hue/Saturation/Lightness components
                M_XYZ           = 1 << 2,   // CIE XYZ components
                M_LAB           = 1 << 3,   // CIE LAB D65 standard components
                M_LCH           = 1 << 4,   // CIE LCH D65 standard components
                M_CMYK          = 1 << 5,   // CMYK color space
            };

            typedef struct rgb_t
            {
                float   R, G, B;
            } rgb_t;

            typedef struct hsl_t
            {
                float   H, S, L;
            } hsl_t;

            typedef struct xyz_t
            {
                float   X, Y, Z;
            } xyz_t;

            typedef struct lab_t
            {
                float   L, A, B;
            } lab_t;

            typedef struct lch_t
            {
                float   L, C, H;
            } lch_t;

            typedef struct cmyk_t
            {
                float   C, M, Y, K;
            } cmyk_t;

        protected:
            mutable rgb_t   rgb;
            mutable hsl_t   hsl;
            mutable xyz_t   xyz;
            mutable lab_t   lab;
            mutable lch_t   lch;
            mutable cmyk_t  cmyk;
            mutable size_t  mask;
            mutable float   A;

            rgb_t          &calc_rgb() const;
            hsl_t          &calc_hsl() const;
            xyz_t          &calc_xyz() const;
            lab_t          &calc_lab() const;
            lch_t          &calc_lch() const;
            cmyk_t         &calc_cmyk() const;

            bool            hsl_to_rgb() const;
            bool            xyz_to_rgb() const;
            bool            lab_to_xyz() const;
            bool            lch_to_lab() const;
            bool            cmyk_to_rgb() const;

        protected:
            static status_t     parse_hex(float *dst, size_t n, char prefix, const char *src, size_t len);
            static status_t     parse_numeric(float *dst, size_t nmin, size_t nmax, const char *prefix, const char *src, size_t len);
            static status_t     parse_cnumeric(float *dst, size_t nmin, size_t nmax, const char *prefix, const char *src, size_t len);
            static ssize_t      format(char *dst, size_t len, size_t tolerance, const float *v, char prefix, bool alpha);
            static inline float clamp(float x);

        // Construction and destruction
        public:
            Color();
            explicit Color(const Color &src);
            explicit Color(const Color *src);
            explicit Color(uint32_t rgb);
            Color(float r, float g, float b);
            Color(float r, float g, float b, float a);
            Color(const Color &src, float a);
            Color(const Color *src, float a);
            Color(uint32_t rgb, float a);

        // Copying and swapping
        public:
            Color          &copy(const Color &c);
            Color          &copy(const Color *c);
            Color          &copy(const Color &c, float a);
            Color          &copy(const Color *c, float a);
            void            swap(Color *src);
            inline void     swap(Color &src)        { swap(&src);               }

        // RGB-related functions
        public:
            // Check that RGB data is currently present without need of implicit conversion
            inline bool     is_rgb() const          { return mask & M_RGB;      }

            // Get color components
            inline float    red() const             { return calc_rgb().R;      }
            inline float    green() const           { return calc_rgb().G;      }
            inline float    blue() const            { return calc_rgb().B;      }

            const Color    &get_rgb(float &r, float &g, float &b) const;
            Color          &get_rgb(float &r, float &g, float &b);
            const Color    &get_rgba(float &r, float &g, float &b, float &a) const;
            const Color    &get_rgbo(float &r, float &g, float &b, float &o) const;
            Color          &get_rgba(float &r, float &g, float &b, float &a);

            uint32_t        rgb24() const;
            uint32_t        rgba32() const;

            // Update color components
            Color          &red(float r);
            Color          &green(float g);
            Color          &blue(float b);

            Color          &set_rgb(float r, float g, float b);
            Color          &set_rgba(float r, float g, float b, float a);

            Color          &set_rgb24(uint32_t v);
            Color          &set_rgba32(uint32_t v);

            // Formatting
            ssize_t         format_rgb(char *dst, size_t len, size_t tolerance = 2) const;
            ssize_t         format_rgb(LSPString *dst, size_t tolerance = 2) const;
            ssize_t         format_rgba(char *dst, size_t len, size_t tolerance = 2) const;
            ssize_t         format_rgba(LSPString *dst, size_t tolerance = 2) const;

            // Parsing
            status_t        parse_rgb(const char *src, size_t len);
            status_t        parse_rgb(const char *src);
            status_t        parse_rgb(const LSPString *src, size_t len)     { return parse_rgb(src->get_utf8(0, len));      }
            status_t        parse_rgb(const LSPString *src)                 { return parse_rgb(src->get_utf8());            }

            status_t        parse_rgba(const char *src, size_t len);
            status_t        parse_rgba(const char *src);
            status_t        parse_rgba(const LSPString *src, size_t len)    { return parse_rgba(src->get_utf8(0, len));     }
            status_t        parse_rgba(const LSPString *src)                { return parse_rgba(src->get_utf8());           }

        // HSL-related functions
        public:
            // Check that HSL data is currently present without need of implicit conversion
            inline bool     is_hsl() const          { return mask & M_HSL;      }

            // Get color components
            inline float    hue() const             { return calc_hsl().H;      }
            inline float    saturation() const      { return calc_hsl().S;      }
            inline float    lightness() const       { return calc_hsl().L;      }
            inline float    hsl_hue() const         { return calc_hsl().H;      }
            inline float    hsl_saturation() const  { return calc_hsl().S;      }
            inline float    hsl_lightness() const   { return calc_hsl().L;      }

            const Color    &get_hsl(float &h, float &s, float &l) const;
            Color          &get_hsl(float &h, float &s, float &l);
            const Color    &get_hsla(float &h, float &s, float &l, float &a) const;
            Color          &get_hsla(float &h, float &s, float &l, float &a);

            uint32_t        hsl24() const;
            uint32_t        hsla32() const;

            // Update color components
            Color          &hue(float h);
            Color          &saturation(float s);
            Color          &lightness(float l);
            Color          &hsl_hue(float h);
            Color          &hsl_saturation(float s);
            Color          &hsl_lightness(float l);

            Color          &set_hsl(float h, float s, float l);
            Color          &set_hsla(float h, float s, float l, float a);

            Color          &set_hsl24(uint32_t v);
            Color          &set_hsla32(uint32_t v);

            // Formatting
            ssize_t         format_hsl(char *dst, size_t len, size_t tolerance = 2) const;
            ssize_t         format_hsl(LSPString *dst, size_t tolerance = 2) const;
            ssize_t         format_hsla(char *dst, size_t len, size_t tolerance = 2) const;
            ssize_t         format_hsla(LSPString *dst, size_t tolerance = 2) const;

            // Parsing
            status_t        parse_hsl(const char *src, size_t len);
            status_t        parse_hsl(const char *src);
            status_t        parse_hsl(const LSPString *src, size_t len)     { return parse_hsl(src->get_utf8(0, len));      }
            status_t        parse_hsl(const LSPString *src)                 { return parse_hsl(src->get_utf8());            }

            status_t        parse_hsla(const char *src, size_t len);
            status_t        parse_hsla(const char *src);
            status_t        parse_hsla(const LSPString *src, size_t len)    { return parse_hsla(src->get_utf8(0, len));     }
            status_t        parse_hsla(const LSPString *src)                { return parse_hsla(src->get_utf8());           }

        // CIE XYZ-related functions (D65 standard)
        public:
            // Check that XYZ data is currently present without need of implicit conversion
            inline bool     is_xyz() const          { return mask & M_XYZ;      }

            // Get color components
            inline float    xyz_x() const           { return calc_xyz().X;      }
            inline float    xyz_y() const           { return calc_xyz().Y;      }
            inline float    xyz_z() const           { return calc_xyz().Z;      }

            const Color    &get_xyz(float &x, float &y, float &z) const;
            Color          &get_xyz(float &x, float &y, float &z);
            const Color    &get_xyza(float &x, float &y, float &z, float &a) const;
            Color          &get_xyza(float &x, float &y, float &z, float &a);

            // Update color components
            Color          &xyz_x(float x);
            Color          &xyz_y(float y);
            Color          &xyz_z(float z);

            Color          &set_xyz(float x, float y, float z);
            Color          &set_xyza(float x, float y, float z, float a);

        // CIE LAB-related functions (D65 standard)
        public:
            // Check that LAB data is currently present without need of implicit conversion
            inline bool     is_lab() const          { return mask & M_LAB;      }

            // Get color components
            inline float    lab_l() const           { return calc_lab().L;      }
            inline float    lab_a() const           { return calc_lab().A;      }
            inline float    lab_b() const           { return calc_lab().B;      }

            const Color    &get_lab(float &l, float &a, float &b) const;
            Color          &get_lab(float &l, float &a, float &b);
            const Color    &get_laba(float &l, float &a, float &b, float &alpha) const;
            Color          &get_laba(float &l, float &a, float &b, float &alpha);

            // Update color components
            Color          &lab_l(float l);
            Color          &lab_a(float a);
            Color          &lab_b(float b);

            Color          &set_lab(float l, float a, float b);
            Color          &set_laba(float l, float a, float b, float alpha);

        // CIE LCH-related functions (D65 standard)
        public:
            // Check that LCH data is currently present without need of implicit conversion
            inline bool     is_lch() const          { return mask & M_LCH;      }
            inline bool     is_hcl() const          { return mask & M_LCH;      }

            // Get color components
            inline float    lch_l() const           { return calc_lch().L;      }
            inline float    lch_c() const           { return calc_lch().C;      }
            inline float    lch_h() const           { return calc_lch().H;      }
            inline float    hcl_l() const           { return calc_lch().L;      }
            inline float    hcl_c() const           { return calc_lch().C;      }
            inline float    hcl_h() const           { return calc_lch().H;      }

            const Color    &get_lch(float &l, float &c, float &h) const;
            Color          &get_lch(float &l, float &c, float &h);
            const Color    &get_lcha(float &l, float &c, float &h, float &alpha) const;
            Color          &get_lcha(float &l, float &c, float &h, float &alpha);
            const Color    &get_hcl(float &h, float &c, float &l) const;
            Color          &get_hcl(float &h, float &c, float &l);
            const Color    &get_hcla(float &h, float &c, float &l, float &alpha) const;
            Color          &get_hcla(float &h, float &c, float &l, float &alpha);

            // Update color components
            Color          &lch_l(float l);
            Color          &lch_c(float c);
            Color          &lch_h(float h);
            Color          &hcl_l(float l);
            Color          &hcl_c(float c);
            Color          &hcl_h(float h);

            Color          &set_lch(float l, float c, float h);
            Color          &set_lcha(float l, float c, float h, float alpha);
            Color          &set_hcl(float h, float c, float l);
            Color          &set_hcla(float h, float c, float l, float alpha);

        // CMYK color space
        public:
            inline bool     is_cmyk() const         { return mask & M_CMYK;     }
            inline float    cyan() const            { return calc_cmyk().C;     }
            inline float    magenta() const         { return calc_cmyk().M;     }
            inline float    yellow() const          { return calc_cmyk().Y;     }
            inline float    black() const           { return calc_cmyk().K;     }

            const Color    &get_cmyk(float &c, float &m, float &y, float &k) const;
            Color          &get_cmyk(float &c, float &m, float &y, float &k);
            const Color    &get_cmyka(float &c, float &m, float &y, float &k, float &alpha) const;
            Color          &get_cmyka(float &c, float &m, float &y, float &k, float &alpha);

            // Update color components
            Color          &cyan(float c);
            Color          &magenta(float m);
            Color          &yellow(float y);
            Color          &black(float k);

            Color          &set_cmyk(float c, float m, float y, float k);
            Color          &set_cmyka(float c, float m, float y, float k, float alpha);

        // Alpha-blending channel
        public:
            inline float    alpha() const           { return A;                 }
            inline float    opacity() const         { return 1.0f - A;          }
            Color          &alpha(float a);
            Color          &opacity(float o);

        // Parsing
        public:
            // Parsing raw data
            status_t        parse3(const char *src, size_t len);
            status_t        parse3(const char *src);
            status_t        parse4(const char *src, size_t len);
            status_t        parse4(const char *src);
            status_t        parse(const char *src, size_t len);
            status_t        parse(const char *src);

            // Parsing LSPString
            inline status_t parse3(const LSPString *src, size_t len)        { return parse3(src->get_utf8(0, len));         }
            inline status_t parse3(const LSPString *src)                    { return parse3(src->get_utf8());               }
            inline status_t parse4(const LSPString *src, size_t len)        { return parse4(src->get_utf8(0, len));         }
            inline status_t parse4(const LSPString *src)                    { return parse4(src->get_utf8());               }

            // Formatting data
            ssize_t         format3(char *dst, size_t len) const;
            ssize_t         format3(LSPString *dst) const;
            ssize_t         format4(char *dst, size_t len) const;
            ssize_t         format4(LSPString *dst) const;

        // Miscellaneous effects
        public:
            Color          &blend(const Color &c, float alpha);
            Color          &blend(float r, float g, float b, float alpha);
            Color          &darken(float amount);
            Color          &lighten(float amount);
            Color          &blend(const Color &c1, const Color &c2, float alpha);
            void            scale_hsl_lightness(float amount);
            void            scale_lch_luminance(float amount);
    };

} /* namespace lsp */

#endif /* LSP_PLUG_IN_RUNTIME_COLOR_H_ */
