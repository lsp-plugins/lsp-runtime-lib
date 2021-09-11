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
                M_RGB           = 1 << 0,
                M_HSL           = 1 << 1
            };

            typedef struct rgb_t
            {
                float   R, G, B;
            } rgb_t;

            typedef struct hsl_t
            {
                float   H, S, L;
            } hsl_t;

        protected:
            mutable rgb_t   rgb;
            mutable hsl_t   hsl;
            mutable size_t  mask;
            mutable float   A;

            rgb_t          &calc_rgb() const;
            hsl_t          &calc_hsl() const;

        protected:
            static status_t     parse(float *dst, size_t n, char prefix, const char *src, size_t len);
            static ssize_t      format(char *dst, size_t len, size_t tolerance, const float *v, char prefix, bool alpha);
            static inline float clamp(float x);

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

        public:
            inline float    red() const             { return calc_rgb().R;   }
            inline float    green() const           { return calc_rgb().G;   }
            inline float    blue() const            { return calc_rgb().B;   }
            inline float    hue() const             { return calc_hsl().H;   }
            inline float    saturation() const      { return calc_hsl().S;   }
            inline float    lightness() const       { return calc_hsl().L;   }
            inline float    hsl_hue() const         { return calc_hsl().H;   }
            inline float    hsl_saturation() const  { return calc_hsl().S;   }
            inline float    hsl_lightness() const   { return calc_hsl().L;   }
            inline float    alpha() const           { return A;              }

            Color          &red(float r);
            Color          &green(float g);
            Color          &blue(float b);
            Color          &hue(float h);
            Color          &saturation(float s);
            Color          &lightness(float l);
            Color          &hsl_hue(float h);
            Color          &hsl_saturation(float s);
            Color          &hsl_lightness(float l);
            Color          &alpha(float a);

            const Color    &get_rgb(float &r, float &g, float &b) const;
            const Color    &get_rgba(float &r, float &g, float &b, float &a) const;
            const Color    &get_hsl(float &h, float &s, float &l) const;
            const Color    &get_hsla(float &h, float &s, float &l, float &a) const;

            Color          &get_rgb(float &r, float &g, float &b);
            Color          &get_rgba(float &r, float &g, float &b, float &a);
            Color          &get_hsl(float &h, float &s, float &l);
            Color          &get_hsla(float &h, float &s, float &l, float &a);

            Color          &set_rgb(float r, float g, float b);
            Color          &set_rgba(float r, float g, float b, float a);
            Color          &set_hsl(float h, float s, float l);
            Color          &set_hsla(float h, float s, float l, float a);

            Color          &blend(const Color &c, float alpha);
            Color          &blend(float r, float g, float b, float alpha);
            Color          &darken(float amount);
            Color          &lighten(float amount);
            Color          &blend(const Color &c1, const Color &c2, float alpha);

            Color          &copy(const Color &c);
            Color          &copy(const Color *c);
            Color          &copy(const Color &c, float a);
            Color          &copy(const Color *c, float a);

            uint32_t        rgb24() const;
            uint32_t        hsl24() const;
            uint32_t        rgba32() const;
            uint32_t        hsla32() const;

            // Checking active color model
            inline bool     is_rgb() const      { return mask & M_RGB; }
            inline bool     is_hsl() const      { return mask & M_HSL; }

            // Setting
            Color          &set_rgb24(uint32_t v);
            Color          &set_rgba32(uint32_t v);
            Color          &set_hsl24(uint32_t v);
            Color          &set_hsla32(uint32_t v);

            // Formatting
            ssize_t         format_rgb(char *dst, size_t len, size_t tolerance = 2) const;
            ssize_t         format_hsl(char *dst, size_t len, size_t tolerance = 2) const;
            ssize_t         format_rgba(char *dst, size_t len, size_t tolerance = 2) const;
            ssize_t         format_hsla(char *dst, size_t len, size_t tolerance = 2) const;

            // Parsing raw data
            status_t        parse3(const char *src, size_t len);
            status_t        parse3(const char *src);
            status_t        parse_rgb(const char *src, size_t len);
            status_t        parse_hsl(const char *src, size_t len);
            status_t        parse_rgb(const char *src);
            status_t        parse_hsl(const char *src);

            status_t        parse4(const char *src, size_t len);
            status_t        parse4(const char *src);
            status_t        parse_rgba(const char *src, size_t len);
            status_t        parse_hsla(const char *src, size_t len);
            status_t        parse_rgba(const char *src);
            status_t        parse_hsla(const char *src);

            // Parsing LSPString
            inline status_t parse3(const LSPString *src, size_t len)        { return parse3(src->get_utf8(0, len));         }
            inline status_t parse3(const LSPString *src)                    { return parse3(src->get_utf8());               }
            status_t        parse_rgb(const LSPString *src, size_t len)     { return parse_rgb(src->get_utf8(0, len));      }
            status_t        parse_hsl(const LSPString *src, size_t len)     { return parse_hsl(src->get_utf8(0, len));      }
            status_t        parse_rgb(const LSPString *src)                 { return parse_rgb(src->get_utf8());            }
            status_t        parse_hsl(const LSPString *src)                 { return parse_hsl(src->get_utf8());            }

            inline status_t parse4(const LSPString *src, size_t len)        { return parse4(src->get_utf8(0, len));         }
            inline status_t parse4(const LSPString *src)                    { return parse4(src->get_utf8());               }
            status_t        parse_rgba(const LSPString *src, size_t len)    { return parse_rgba(src->get_utf8(0, len));     }
            status_t        parse_hsla(const LSPString *src, size_t len)    { return parse_hsla(src->get_utf8(0, len));     }
            status_t        parse_rgba(const LSPString *src)                { return parse_rgba(src->get_utf8());           }
            status_t        parse_hsla(const LSPString *src)                { return parse_hsla(src->get_utf8());           }

        public:
            void            scale_lightness(float amount);
            void            swap(Color *src);
    };

} /* namespace lsp */

#endif /* LSP_PLUG_IN_RUNTIME_COLOR_H_ */
