/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 25 янв. 2026 г.
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

#include <lsp-plug.in/fmt/xpm/xpm.h>
#include <lsp-plug.in/io/InFileStream.h>
#include <lsp-plug.in/io/InMemoryStream.h>
#include <lsp-plug.in/io/InBufStream.h>
#include <lsp-plug.in/mm/Bitmap.h>

namespace lsp
{
    namespace mm
    {
        static inline bool is_exact_copy(pixel_format_t a, pixel_format_t b)
        {
            if (a == b)
                return true;
            switch (a)
            {
                case PIXFMT_G1:         return b == PIXFMT_A1;
                case PIXFMT_G2:         return b == PIXFMT_A2;
                case PIXFMT_G4:         return b == PIXFMT_A4;
                case PIXFMT_G8:         return b == PIXFMT_A8;
                case PIXFMT_A1:         return b == PIXFMT_G1;
                case PIXFMT_A2:         return b == PIXFMT_G2;
                case PIXFMT_A4:         return b == PIXFMT_G4;
                case PIXFMT_A8:         return b == PIXFMT_G8;
                default:
                    break;
            }
            return false;
        }

        Bitmap::Bitmap() noexcept
        {
            pData       = NULL;
            nRows       = 0;
            nCols       = 0;
            nStride     = 0;
            enFormat    = PIXFMT_R8G8B8A8;
        }

        Bitmap::Bitmap(Bitmap && src) noexcept
        {
            pData       = lsp::exchange(src.pData, static_cast<uint8_t *>(NULL));
            nRows       = lsp::exchange(src.nRows, 0);
            nCols       = lsp::exchange(src.nCols, 0);
            nStride     = lsp::exchange(src.nStride, 0);
            enFormat    = lsp::exchange(src.enFormat, PIXFMT_R8G8B8A8);
        }

        Bitmap::~Bitmap()
        {
            if (pData != NULL)
            {
                free(pData);
                pData       = NULL;
            }
        }

        inline size_t Bitmap::calc_bytes_per_row(pixel_format_t format, size_t cols) noexcept
        {
            const size_t bpp        = device_bits_per_pixel(format);
            return (bpp * cols + 7) >> 3;
        }

        inline size_t Bitmap::calc_stride(pixel_format_t format, size_t cols) noexcept
        {
            const size_t bpp        = device_bits_per_pixel(format);
            return (bpp <= 8) ? (bpp * cols + 7) >> 3 : (bpp * cols + 31) >> 5;
        }

        Bitmap & Bitmap::operator = (Bitmap && src) noexcept
        {
            pData       = lsp::exchange(src.pData, static_cast<uint8_t *>(NULL));
            nRows       = lsp::exchange(src.nRows, 0);
            nCols       = lsp::exchange(src.nCols, 0);
            nStride     = lsp::exchange(src.nStride, 0);
            enFormat    = lsp::exchange(src.enFormat, PIXFMT_R8G8B8A8);

            return *this;
        }

        void Bitmap::swap(Bitmap & src) noexcept
        {
            lsp::swap(pData, src.pData);
            lsp::swap(nRows, src.nRows);
            lsp::swap(nCols, src.nCols);
            lsp::swap(nStride, src.nStride);
            lsp::swap(enFormat, src.enFormat);
        }

        void Bitmap::swap(Bitmap * src) noexcept
        {
            lsp::swap(pData, src->pData);
            lsp::swap(nRows, src->nRows);
            lsp::swap(nCols, src->nCols);
            lsp::swap(nStride, src->nStride);
            lsp::swap(enFormat, src->enFormat);
        }

        void Bitmap::reset()
        {
            if (pData != NULL)
            {
                free(pData);
                pData       = NULL;
            }
            nRows       = 0;
            nCols       = 0;
        }

        status_t Bitmap::init(pixel_format_t format, size_t rows, size_t cols)
        {
            if (format == PIXFMT_DEFAULT)
                return STATUS_INVALID_VALUE;
            if ((rows <= 0) || (cols <= 0))
            {
                reset();
                return STATUS_OK;
            }

            const size_t stride     = calc_stride(format, cols);
            const size_t to_alloc   = stride * rows;
            uint8_t * data          = static_cast<uint8_t *>(malloc(to_alloc));
            if (data == NULL)
                return STATUS_NO_MEM;
            lsp_finally {
                if (data != NULL)
                    free(data);
            };

            // Cleanup data
            bzero(data, to_alloc);

            // Commit data
            lsp::swap(pData, data);
            nRows       = rows;
            nCols       = cols;
            nStride     = stride;
            enFormat    = format;

            return STATUS_OK;
        }

        status_t Bitmap::load(const char *path, pixel_format_t format, IColorMap *map)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::InFileStream ifs;
            status_t res = ifs.open(path);
            if (res != STATUS_OK)
                return res;

            res = update_status(res, do_load(&ifs, format, map));
            return update_status(res, ifs.close());
        }

        status_t Bitmap::load(const LSPString *path, pixel_format_t format, IColorMap *map)
        {
            return (path != NULL) ? load(*path, format, map) : STATUS_BAD_ARGUMENTS;
        }

        status_t Bitmap::load(const LSPString & path, pixel_format_t format, IColorMap *map)
        {
            io::InFileStream ifs;
            status_t res = ifs.open(&path);
            if (res != STATUS_OK)
                return res;

            res = update_status(res, do_load(&ifs, format, map));
            return update_status(res, ifs.close());
        }

        status_t Bitmap::load(const io::Path *path, pixel_format_t format, IColorMap *map)
        {
            return (path != NULL) ? load(*path, format, map) : STATUS_BAD_ARGUMENTS;
        }

        status_t Bitmap::load(const io::Path & path, pixel_format_t format, IColorMap *map)
        {
            io::InFileStream ifs;
            status_t res = ifs.open(&path);
            if (res != STATUS_OK)
                return res;

            res = update_status(res, do_load(&ifs, format, map));
            return update_status(res, ifs.close());
        }

        status_t Bitmap::load(const void *data, size_t size, pixel_format_t format, IColorMap *map)
        {
            if (data == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::InMemoryStream ims;
            ims.wrap(data, size);

            status_t res = do_load(&ims, format, map);
            return update_status(res, ims.close());
        }

        status_t Bitmap::load(io::IInStream *is, pixel_format_t format, IColorMap *map)
        {
            io::InBufStream ibs;
            status_t res = ibs.wrap(is, WRAP_NONE);
            if (res != STATUS_OK)
                return res;

            res = update_status(res, do_load(&ibs, format, map));
            return update_status(res, ibs.close());
        }


        status_t Bitmap::do_load(io::IInStream *is, pixel_format_t format, IColorMap *map)
        {
            status_t res = load_xpm(is, format, map);
            if ((res == STATUS_OK) || (res != STATUS_UNSUPPORTED_FORMAT))
                return res;

            return STATUS_UNSUPPORTED_FORMAT;
        }

        status_t Bitmap::resize(size_t rows, size_t cols)
        {
            if ((nRows == rows) && (nCols == cols))
                return STATUS_OK;

            // Compute parameters of the new bitmap
            const size_t stride     = calc_stride(enFormat, cols);
            const size_t to_alloc   = stride * rows;
            uint8_t * data          = static_cast<uint8_t *>(malloc(to_alloc));
            if (data == NULL)
                return STATUS_NO_MEM;
            lsp_finally {
                if (data != NULL)
                    free(data);
            };

            // Preserve previous picture if possible
            if ((pData != NULL) && (nRows > 0) && (nCols > 0))
            {
                const size_t to_copy    = lsp_min(stride, nStride);
                const size_t to_rows    = lsp_min(rows, nRows);

                for (size_t i=0; i<to_rows; ++i)
                {
                    memcpy(&data[i*stride], &pData[i*nStride], to_copy);
                    bzero(&data[i*stride + to_copy], to_copy - nStride);
                }
                bzero(&data[to_rows * stride], (rows - nRows) * stride);
            }
            else
                bzero(data, to_alloc);

            lsp::swap(pData, data);
            nRows       = rows;
            nCols       = cols;
            nStride     = stride;

            return STATUS_OK;
        }

        status_t Bitmap::convert(Bitmap & dst, pixel_format_t format)
        {
            if (enFormat == format)
                return STATUS_OK;
            else if (format == PIXFMT_DEFAULT)
                return STATUS_INVALID_VALUE;

            // Check that no changes are required to be made
            if ((nRows <= 0) || (nCols <= 0) || (is_exact_copy(enFormat, format)))
            {
                enFormat    = format;
                return STATUS_OK;
            }

            // Get pixel conversion function
            const pixel_conversion_t conversion = pixel_convert_function(format, enFormat);
            if (conversion == NULL)
                return STATUS_UNSUPPORTED_FORMAT;

            // Check if we need to perform reallocation
            const size_t old_bpp = device_bits_per_pixel(enFormat);
            const size_t new_bpp = device_bits_per_pixel(format);
            if (old_bpp == new_bpp)
            {
                // Perform conversion
                for (size_t r = 0; r < nRows; ++r)
                    conversion(&pData[r*nStride], &pData[r*nStride], nCols);
            }
            else
            {
                // Need to re-allocate memory
                const size_t bpr        = calc_bytes_per_row(enFormat, nCols);
                const size_t stride     = calc_stride(enFormat, nCols);
                const size_t to_alloc   = stride * nRows;
                uint8_t * data          = static_cast<uint8_t *>(malloc(to_alloc));
                if (data == NULL)
                    return STATUS_NO_MEM;
                lsp_finally {
                    if (data != NULL)
                        free(data);
                };

                // Perform conversion
                for (size_t r = 0; r < nRows; ++r)
                {
                    conversion(&data[r*stride], &pData[r*nStride], nCols);
                    bzero(&data[r*stride + bpr], stride - bpr);
                }

                // Commit new data
                lsp::swap(pData, data);
            }

            enFormat    = format;

            return STATUS_NOT_IMPLEMENTED;
        }

        status_t Bitmap::convert(Bitmap *dst, pixel_format_t format)
        {
            return (dst != NULL) ? convert(*dst, format) : STATUS_BAD_ARGUMENTS;
        }

        status_t Bitmap::convert(pixel_format_t format)
        {
            return convert(*this, format);
        }


        status_t Bitmap::load_xpm(io::IInStream *is, pixel_format_t format, IColorMap *map)
        {
            // TODO

            return STATUS_NOT_IMPLEMENTED;
        }

    } /* namespace mm */
} /* namespace lsp */


