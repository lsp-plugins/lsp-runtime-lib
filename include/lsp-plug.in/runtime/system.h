/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 17 мар. 2019 г.
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

#ifndef LSP_PLUG_IN_RUNTIME_SYSTEM_H_
#define LSP_PLUG_IN_RUNTIME_SYSTEM_H_

#include <lsp-plug.in/runtime/version.h>
#include <lsp-plug.in/runtime/LSPString.h>
#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/io/Path.h>

namespace lsp
{
    namespace system
    {
        /**
         * Time information
         */
        typedef struct time_t
        {
            ssize_t     seconds;    /* The value in seconds */
            ssize_t     nanos;      /* The value in nanoseconds between 0 and 10^9-1 */
        } time_t;

        /**
         * System timestamp in milliseconds
         */
        typedef uint64_t time_millis_t;

        /**
         * Local time information
         */
        typedef struct localtime_t
        {
            int32_t     year;       /* Year */
            uint8_t     month;      /* Month, starting with 1 */
            uint8_t     mday;       /* Day of month, starting from 1 */
            uint8_t     wday;       /* Day of week, starting from 1 */
            uint8_t     hour;       /* Hour of a day, 0-23 */
            uint8_t     min;        /* Minute of an hour, 0-59 */
            uint8_t     sec;        /* Second of a minute, 0-59 */
            uint32_t    nanos;      /* Number of nanoseconds */
        } localtime_t;

        /**
         * Get environment variable
         * @param name environment variable name
         * @param dst string to store environment variable value, NULL for check-only
         * @return status of operation or STATUS_NOT_FOUND if there is no environment variable
         */
        status_t get_env_var(const LSPString *name, LSPString *dst);

        /**
         * Get environment variable
         * @param name environment variable name in UTF-8
         * @param dst string to store environment variable value, NULL for check-only
         * @return status of operation or STATUS_NOT_FOUND if there is no environment variable
         */
        status_t get_env_var(const char *name, LSPString *dst);

        /**
         * Set environment variable
         * @param name environment variable name
         * @param value environment variable value, NULL value deletes the variable
         * @return status of operation or STATUS_NOT_FOUND if there is no environment variable
         */
        status_t set_env_var(const LSPString *name, const LSPString *value);

        /**
         * Set environment variable
         * @param name environment variable name in UTF-8
         * @param value environment variable value in UTF-8, NULL value deletes the variable
         * @return status of operation or STATUS_NOT_FOUND if there is no environment variable
         */
        status_t set_env_var(const char *name, const char *value);

        /**
         * Set environment variable
         * @param name environment variable name in UTF-8
         * @param value environment variable value, NULL value deletes the variable
         * @return status of operation or STATUS_NOT_FOUND if there is no environment variable
         */
        status_t set_env_var(const char *name, const LSPString *value);

        /**
         * Remove environment variable
         * @param name variable to remove in UTF-8
         * @return status of operation
         */
        status_t remove_env_var(const char *name);

        /**
         * Remove environment variable
         * @param name variable to remove
         * @return status of operation
         */
        status_t remove_env_var(const LSPString *name);

        /**
         * Get current user's home directory
         * @param homedir pointer to string to store home directory path
         * @return status of operation
         */
        status_t get_home_directory(LSPString *homedir);

        /**
         * Get current user's home directory
         * @param homedir pointer to string to store home directory path
         * @return status of operation
         */
        status_t get_home_directory(io::Path *homedir);

        /**
         * Get user's local configuration path
         * @param path string to store user's configuration path
         * @return status of operation
         */
        status_t get_user_config_path(LSPString *path);

        /**
         * Get user's local configuration path
         * @param path string to store user's configuration path
         * @return status of operation
         */
        status_t get_user_config_path(io::Path *path);

        /**
         * Get current high-precision time
         * @param time pointer to structure to store time value
         */
        void get_time(time_t *time);

        /**
         * Get current time in milliseconds
         * @return current time in milliseconds
         */
        time_millis_t get_time_millis();

        /**
         * Convert time structure to the local time
         * @param ltime pointer to store the result
         * @param time time structure to convert, use current time if NULL
         */
        void get_localtime(localtime_t *local, const time_t *time = NULL);

        /**
         * Sleep for a specified amount of milliseconds
         * @param delay the amount of milliseconds to sleep
         */
        status_t sleep_msec(size_t delay);

        /**
         * Get temporary directory
         * @param path pointer to store result
         * @return status of operation
         */
        status_t get_temporary_dir(LSPString *path);

        /**
         * Get temporary directory
         * @param path pointer to store result
         * @return status of operation
         */
        status_t get_temporary_dir(io::Path *path);

        /**
         * Get current directiory
         * @param path path to store current directory
         * @return status of operation
         */
        status_t get_current_dir(LSPString *path);

        /**
         * Get current directiory
         * @param path path to store current directory
         * @return status of operation
         */
        status_t get_current_dir(io::Path *path);

        /**
         * Follow ther URL (open in the matching program)
         * @param url URL to follow
         * @return status of operation
         */
        status_t follow_url(const char *url);

        /**
         * Follow ther URL (open in the matching program)
         * @param url URL to follow
         * @return status of operation
         */
        status_t follow_url(const LSPString *url);
    }
}


#endif /* LSP_PLUG_IN_RUNTIME_SYSTEM_H_ */
