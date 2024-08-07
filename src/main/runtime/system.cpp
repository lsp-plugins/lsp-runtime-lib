/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/common/debug.h>
#include <lsp-plug.in/io/Dir.h>
#include <lsp-plug.in/ipc/Process.h>
#include <lsp-plug.in/lltl/darray.h>
#include <lsp-plug.in/runtime/system.h>
#include <lsp-plug.in/stdlib/stdlib.h>

#ifdef PLATFORM_WINDOWS
    #include <windows.h>
    #include <shellapi.h>
    #include <synchapi.h>
    #include <sysinfoapi.h>
    #include <winnetwk.h>
#endif /* PLATFORM_WINDOWS */

#if defined PLATFORM_POSIX
    #include <errno.h>
    #include <sys/stat.h>
    #include <sys/time.h>
    #include <time.h>
    #include <unistd.h>
#endif /* PLATFORM_POSIX */

#if defined PLATFORM_LINUX
    #include <mntent.h>
#endif /* PLATFORM_LINUX */

#if defined PLATFORM_BSD
    #include <sys/mount.h>
    #include <sys/param.h>
    #include <sys/ucred.h>
#endif /* PLATFORM_BSD */

namespace lsp
{
    namespace system
    {
        status_t get_env_var(const LSPString *name, LSPString *dst)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;

#ifdef PLATFORM_WINDOWS
            const lsp_utf16_t *nname = name->get_utf16();
            if (nname == NULL)
                return STATUS_NO_MEM;

            DWORD bufsize = ::GetEnvironmentVariableW(nname, NULL, 0);
            if (bufsize == 0)
            {
                if (::GetLastError() == ERROR_ENVVAR_NOT_FOUND)
                    return STATUS_NOT_FOUND;
                return STATUS_UNKNOWN_ERR;
            }
            else if (dst == NULL)
                return STATUS_OK;

            lsp_utf16_t *buf = reinterpret_cast<lsp_utf16_t *>(::malloc(bufsize * sizeof(lsp_utf16_t)));
            if (buf == NULL)
                return STATUS_NO_MEM;
            bufsize = ::GetEnvironmentVariableW(nname, buf, bufsize);
            if (bufsize == 0)
            {
                ::free(buf);
                return STATUS_UNKNOWN_ERR;
            }

            bool res = dst->set_utf16(buf, bufsize);
            ::free(buf);
            return (res) ? STATUS_OK : STATUS_NO_MEM;
#else
            const char *nname = name->get_native();
            if (nname == NULL)
                return STATUS_NO_MEM;

#ifdef _GNU_SOURCE
            char *var = secure_getenv(nname);
#else
            char *var = getenv(nname);
#endif
            if (var == NULL)
                return STATUS_NOT_FOUND;
            if (dst != NULL)
            {
                if (!dst->set_native(var))
                    return STATUS_NO_MEM;
            }
            return STATUS_OK;
#endif /* PLATFORM_WINDOWS */
        }

        status_t get_env_var(const char *name, LSPString *dst)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString sname;
            if (!sname.set_utf8(name))
                return STATUS_NO_MEM;
            return get_env_var(&sname, dst);
        }

        status_t set_env_var(const LSPString *name, const LSPString *value)
        {
#ifdef PLATFORM_WINDOWS
            const lsp_utf16_t *nname = name->get_utf16();
            if (nname == NULL)
                return STATUS_NO_MEM;

            if (value != NULL)
            {
                const lsp_utf16_t *nvalue = value->get_utf16();
                if (nvalue == NULL)
                    return STATUS_NO_MEM;
                if (::SetEnvironmentVariableW(nname, nvalue))
                    return STATUS_OK;
            }
            else
            {
                if (::SetEnvironmentVariableW(nname, NULL))
                    return STATUS_OK;
            }
            return STATUS_UNKNOWN_ERR;
#else
            const char *nname = name->get_native();
            if (nname == NULL)
                return STATUS_NO_MEM;

            int res;
            if (value != NULL)
            {
                const char *nvalue = value->get_native();
                if (nvalue == NULL)
                    return STATUS_NO_MEM;
                res = ::setenv(nname, nvalue, 1);
            }
            else
                res = ::unsetenv(nname);

            if (res == 0)
                return STATUS_OK;
            switch (res)
            {
                case EINVAL: return STATUS_INVALID_VALUE;
                case ENOMEM: return STATUS_NO_MEM;
                default: break;
            }
            return STATUS_UNKNOWN_ERR;
#endif /* PLATFORM_WINDOWS */
        }

        status_t set_env_var(const char *name, const char *value)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString sname;
            if (!sname.set_utf8(name))
                return STATUS_NO_MEM;
            if (value == NULL)
                return set_env_var(&sname, NULL);

            LSPString svalue;
            if (!svalue.set_utf8(value))
                return STATUS_NO_MEM;
            return set_env_var(&sname, &svalue);
        }

        status_t set_env_var(const char *name, const LSPString *value)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString sname;
            if (!sname.set_utf8(name))
                return STATUS_NO_MEM;
            return set_env_var(&sname, value);
        }

        status_t remove_env_var(const LSPString *name)
        {
            const char *nname = name->get_native();
            if (nname == NULL)
                return STATUS_NO_MEM;

#ifdef PLATFORM_WINDOWS
            if (SetEnvironmentVariable(nname, NULL))
                return STATUS_OK;
            return STATUS_UNKNOWN_ERR;
#else
            int res = ::unsetenv(nname);
            if (res == 0)
                return STATUS_OK;
            switch (res)
            {
                case EINVAL: return STATUS_INVALID_VALUE;
                case ENOMEM: return STATUS_NO_MEM;
                default: break;
            }
            return STATUS_UNKNOWN_ERR;
#endif /* PLATFORM_WINDOWS */
        }

        status_t remove_env_var(const char *name)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString sname;
            if (!sname.set_utf8(name))
                return STATUS_NO_MEM;
            return remove_env_var(&sname);
        }

        status_t get_home_directory(LSPString *homedir)
        {
            if (homedir == NULL)
                return STATUS_BAD_ARGUMENTS;
#ifdef PLATFORM_WINDOWS
            LSPString drv, path;
            status_t res = get_env_var("HOMEDRIVE", &drv);
            if (res != STATUS_OK)
                return res;
            res = get_env_var("HOMEPATH", &path);
            if (res != STATUS_OK)
                return res;
            if (!drv.append(&path))
                return STATUS_NO_MEM;

            homedir->take(&drv);
            return STATUS_OK;
#else
            return get_env_var("HOME", homedir);
#endif /* PLATFORM_WINDOWS */
        }

        status_t get_home_directory(io::Path *homedir)
        {
            if (homedir == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString path;
            status_t res = get_home_directory(&path);
            if (res != STATUS_OK)
                return res;
            return homedir->set(&path);
        }

        status_t get_user_config_path(LSPString *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString upath;

#ifdef PLATFORM_WINDOWS
            status_t res = get_env_var("LOCALAPPDATA", &upath);
            if (res != STATUS_OK)
            {
                res = get_env_var("APPDATA", &upath);
                if (res != STATUS_OK)
                {
                    res = get_home_directory(&upath);
                    if (res != STATUS_OK)
                        return res;
                    if (!upath.append_ascii(FILE_SEPARATOR_S ".config"))
                        return STATUS_NO_MEM;
                }
            }
#else
            status_t res = get_env_var("HOME", &upath);
            if (res != STATUS_OK)
                return res;
            if (!upath.append_ascii(FILE_SEPARATOR_S ".config"))
                return STATUS_NO_MEM;
#endif /* PLATFORM_WINDOWS */

            path->swap(&upath);
            return STATUS_OK;
        }

        status_t get_user_config_path(io::Path *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString upath;
            status_t res = get_user_config_path(&upath);
            if (res != STATUS_OK)
                return res;
            return path->set(&upath);
        }

#ifdef PLATFORM_WINDOWS
        void get_time(time_t *time)
        {
            FILETIME t;
            ::GetSystemTimeAsFileTime(&t);
            uint64_t itime  = (uint64_t(t.dwHighDateTime) << 32) | t.dwLowDateTime;
            time->seconds   = itime / 10000000;
            time->nanos     = (itime % 10000000) * 100;
        }

        time_millis_t get_time_millis()
        {
            FILETIME t;
            ::GetSystemTimeAsFileTime(&t);
            uint64_t itime  = (uint64_t(t.dwHighDateTime) << 32) | t.dwLowDateTime;
            return itime / 10000;
        }

        void get_localtime(localtime_t *local, const time_t *time)
        {
            SYSTEMTIME t;

            // Get system file time into tmp
            if (time != NULL)
            {
                FILETIME tmp, stime;
                uint64_t itime      = (time->seconds * 10000000) | (time->nanos / 100);
                tmp.dwHighDateTime  = DWORD(itime >> 32);
                tmp.dwLowDateTime   = DWORD(itime);

                ::FileTimeToLocalFileTime(&tmp, &stime);
                ::FileTimeToSystemTime(&stime, &t);
            }
            else
                ::GetLocalTime(&t);

            // Decode
            local->year     = t.wYear;
            local->month    = t.wMonth;
            local->mday     = t.wDay;
            local->wday     = t.wDayOfWeek;
            local->hour     = t.wHour;
            local->min      = t.wMinute;
            local->sec      = t.wSecond;
            local->nanos    = time->nanos;
        }

        status_t sleep_msec(size_t delay)
        {
            HANDLE timer;
            LARGE_INTEGER li;

            timer = CreateWaitableTimerW(NULL, TRUE, NULL);
            if (timer == NULL)
                return STATUS_UNKNOWN_ERR;

            while (delay > 0)
            {
                // Relative time, computed with precision of 100 ns
                size_t period   = lsp_min(0x10000000U, delay);
                li.QuadPart     = - LONGLONG(period) * 10000;

                if (!SetWaitableTimer(timer, &li, 0, NULL, NULL, FALSE))
                {
                    CloseHandle(timer);
                    return STATUS_UNKNOWN_ERR;
                }

                WaitForSingleObject(timer, INFINITE);
                delay          -= period;
            }

            CloseHandle(timer);

            return STATUS_OK;
        }
#else
        void get_time(time_t *time)
        {
            struct timespec t;
            ::clock_gettime(CLOCK_REALTIME, &t);

            time->seconds   = t.tv_sec;
            time->nanos     = t.tv_nsec;
        }

        time_millis_t get_time_millis()
        {
            struct timespec t;
            ::clock_gettime(CLOCK_REALTIME, &t);
            return time_millis_t(t.tv_sec) * 1000 + time_millis_t(t.tv_nsec) / 1000000;
        }

        void get_localtime(localtime_t *local, const time_t *time)
        {
            // Store actual time to timespec struct
            struct timespec stime;

            if (time != NULL)
            {
                stime.tv_sec        = time->seconds;
                stime.tv_nsec       = time->nanos;
            }
            else
                ::clock_gettime(CLOCK_REALTIME, &stime);

            // Now convert the struct to local time
            struct tm *t;
        #if defined(_POSIX_C_SOURCE) || defined(_BSD_SOURCE) || defined(_SVID_SOURCE)
            struct tm ctime;
            t               = localtime_r(&stime.tv_sec, &ctime);
        #else
            t               = localtime(&stime.tv_sec);
        #endif

            local->year     = t->tm_year + 1900;
            local->month    = t->tm_mon + 1;
            local->mday     = t->tm_mday + 1;
            local->wday     = t->tm_wday + 1;
            local->hour     = t->tm_hour;
            local->min      = t->tm_min;
            local->sec      = t->tm_sec;
            local->nanos    = stime.tv_nsec;
        }

        status_t sleep_msec(size_t delay)
        {
            if (delay <= 0)
                return STATUS_OK;

            time_millis_t ctime = get_time_millis();
            const time_millis_t dtime = ctime + delay;

            while (ctime < dtime)
            {
                struct timespec req, rem;
                size_t delta    = dtime - ctime;

                req.tv_nsec     = (delta % 1000) * 1000000;
                req.tv_sec      = delta / 1000;
                rem.tv_nsec     = 0;
                rem.tv_sec      = 0;

                // Perform nanosleep for the specific period of time.
                // If function succeeded and waited the whole desired period
                // of time, it should return 0.
                if (::nanosleep(&req, &rem) == 0)
                    break;

                // If the sleep was interrupted, we need to update
                // the remained number of time to sleep.
                switch (errno)
                {
                    case EINTR:
                        req = rem;
                        break;
                    default:
                        return STATUS_UNKNOWN_ERR;
                }

                // Update current time
                ctime           = get_time_millis();
            }

            return STATUS_OK;
        }
#endif /* PLATFORM_WINDOWS */

#ifdef PLATFORM_WINDOWS
        status_t get_system_temporary_dir(LSPString *path)
        {
            LSPString tmp;

            if (get_env_var("SYSTEMROOT", &tmp) != STATUS_OK)
                return STATUS_NOT_FOUND;

            if (!tmp.append(FILE_SEPARATOR_C))
                return STATUS_NO_MEM;
            if (!tmp.append_ascii("Temp"))
                return STATUS_NO_MEM;

            tmp.swap(path);
            return STATUS_OK;
        }

        status_t get_system_temporary_dir(io::Path *path)
        {
            LSPString tmp;
            status_t res = get_system_temporary_dir(&tmp);
            return (res == STATUS_OK) ? path->set(&tmp) : res;
        }

        status_t get_temporary_dir(LSPString *path)
        {
            if (get_env_var("TEMP", path) == STATUS_OK)
                return STATUS_OK;

            if (get_env_var("TMP", path) == STATUS_OK)
                return STATUS_OK;

            return get_system_temporary_dir(path);
        }

        status_t get_temporary_dir(io::Path *path)
        {
            LSPString tmp;
            status_t res = get_temporary_dir(&tmp);
            return (res == STATUS_OK) ? path->set(&tmp) : res;
        }

        status_t get_user_login(LSPString *user)
        {
            return STATUS_NOT_IMPLEMENTED;
        }
#else
        status_t get_user_login(LSPString *user)
        {
            size_t capacity = 0x40;
            char *buf       = reinterpret_cast<char *>(malloc(capacity));
            if (buf == NULL)
                return STATUS_NO_MEM;
            lsp_finally {
                free(buf);
            };

            while (true)
            {
                int res = getlogin_r(buf, capacity);
                switch (res)
                {
                    case 0:
                        return (user->set_native(buf)) ? STATUS_OK : STATUS_NO_MEM;
                    case ERANGE:
                        break;
                    case EMFILE:
                    case ENFILE:
                    case ENXIO:
                    case ENOTTY:
                        return STATUS_IO_ERROR;
                    case ENOENT:
                        return STATUS_NOT_FOUND;
                    case ENOMEM:
                        return STATUS_NO_MEM;
                    default:
                        return STATUS_UNKNOWN_ERR;
                }

                // Re-allocate data
                capacity      <<= 1;
                char *new_buf   = reinterpret_cast<char *>(realloc(buf, capacity));
                if (new_buf == NULL)
                    return STATUS_NO_MEM;
                buf             = new_buf;
            }
        }

        status_t get_system_temporary_dir(LSPString *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            return (path->set_ascii("/tmp")) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t get_system_temporary_dir(io::Path *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            return path->set("/tmp");
        }

        status_t get_temporary_dir(LSPString *path)
        {
            return get_system_temporary_dir(path);
        }

        status_t get_temporary_dir(io::Path *path)
        {
            return get_system_temporary_dir(path);
        }
#endif /* PLATFORM_WINDOWS */

        status_t get_current_dir(LSPString *path)
        {
            return io::Dir::get_current(path);
        }

        status_t get_current_dir(io::Path *path)
        {
            return io::Dir::get_current(path);
        }

        status_t follow_url(const char *url)
        {
            LSPString turl;
            if (!turl.set_utf8(url))
                return STATUS_NO_MEM;
            return follow_url(&turl);
        }

        status_t follow_url(const LSPString *url)
        {
        #ifdef PLATFORM_WINDOWS
            ::ShellExecuteW(
                NULL,               // Not associated with window
                L"open",            // Open hyperlink
                url->get_utf16(),   // The file to execute
                NULL,               // Parameters
                NULL,               // Directory
                SW_SHOWNORMAL       // Show command
            );
        #else
            status_t res;
            ipc::Process p;

            if ((res = p.set_command("xdg-open")) != STATUS_OK)
                return STATUS_OK;
            if ((res = p.add_arg(url)) != STATUS_OK)
                return STATUS_OK;
            if ((res = p.launch()) != STATUS_OK)
                return STATUS_OK;
            p.wait();
        #endif /* PLATFORM_WINDOWS */

            return STATUS_OK;
        }

        static inline bool match_string(const LSPString *s, const char **list)
        {
            for (; *list != NULL; ++list)
                if (s->equals_ascii(*list))
                    return true;
            return false;
        }

        static inline bool is_dummy_fs(const LSPString *fs_type, bool bind)
        {
            static const char *dummy_fs_types[] =
            {
                "autofs",
                "proc",
                "debugfs",
                "devpts",
                "fusectl",
                "fuse.portal",
                "mqueue",
                "rpc_pipefs",
                "sysfs",
                "devfs",
                "kernfs",
                "ignore",
                NULL
            };

            if ((fs_type->equals_ascii("bind")) && (!bind))
                return true;

            return match_string(fs_type, dummy_fs_types);
        }

        static inline bool is_remote_fs(const LSPString *fs_type, const LSPString *fs_name)
        {
        #ifdef PLATFORM_WINDOWS
            // TODO
        #endif /* PLATFORM_WINDOWS */

            static const char *network_fs_types[] =
            {
                "smbfs",
                "smb3",
                "cifs",
                "nfs",
                NULL
            };

            static const char *network_fs_names[] =
            {
                "acfs",
                "afs",
                "coda",
                "auristorfs",
                "fhgfs",
                "gpfs",
                "ibrix",
                "ocfs2",
                "vxfs",
                "nfs",
                "-hosts",
                NULL
            };

            if (fs_name->index_of(':') >= 0)
                return true;
            if (fs_name->starts_with_ascii("\\\\"))
                return match_string(fs_type, network_fs_types);

            return match_string(fs_name, network_fs_names);
        }

    #ifdef PLATFORM_POSIX
        bool is_posix_drive(const LSPString *path)
        {
            if (!path->starts_with(FILE_SEPARATOR_C))
                return false;

            struct stat st;
            if (::stat(path->get_native(), &st) != 0)
                return false;

            int mode = st.st_mode & S_IFMT;
            return (mode == S_IFBLK) || (mode == S_IFCHR);
        }
    #endif /* PLATFORM_POSIX */

    #ifdef PLATFORM_LINUX
        static inline char *move_forward(char *s, char *end, size_t n)
        {
            while (n--)
            {
                s = static_cast<char *>(memchr(s, ' ', end - s));
                if (s == NULL)
                    break;
                ++s;
            }
            return s;
        }

        static inline status_t read_field(LSPString *dst, char *s, char *end)
        {
            char *e = static_cast<char *>(memchr(s, ' ', end - s));
            if (e == NULL)
                return STATUS_BAD_FORMAT;

            // Process string in-place to remove some escapings
            size_t v    = 0;
            size_t l    = e - s;
            for (size_t i = 0; i < l; i++)
            {
                if ((s[i] == '\\') && ((i + 4) < l) &&
                    (s[i + 1] >= '0') && (s[i + 1] <= '3') &&
                    (s[i + 2] >= '0') && (s[i + 2] <= '7') &&
                    (s[i + 3] >= '0') && (s[i + 3] <= '7'))
                {
                    s[v++]  = (s[i + 1] - '0') * 64 + (s[i + 2] - '0') * 8 + (s[i + 3] - '0');
                    i += 3;
                }
                else
                    s[v++]  = s[i];
            }

            return (dst->set_utf8(s, v)) ? STATUS_OK : STATUS_NO_MEM;
        }

        static status_t read_linux_mountinfo(lltl::parray<volume_info_t> *volumes)
        {
            status_t res;
            lltl::parray<volume_info_t> list;
            lsp_finally { free_volume_info(&list); };

            // Open the file
            FILE *fd    = fopen("/proc/self/mountinfo", "r");
            if (fd == NULL)
                return STATUS_NOT_SUPPORTED;
            lsp_finally { fclose(fd); };

            // Line
            char *line = NULL;
            size_t line_cap = 0;
            lsp_finally {
                if (line != NULL)
                    free(line);
            };

            // Read the mountinfo file
            // 36 35 98:0 /mnt1 /mnt2 rw,noatime master:1 - ext3 /dev/root rw,errors=continue
            // (1)(2)(3)   (4)   (5)      (6)      (7)   (8) (9)   (10)         (11)
            //
            // (1) mount ID:  unique identifier of the mount (may be reused after umount)
            // (2) parent ID:  ID of parent (or of self for the top of the mount tree)
            // (3) major:minor:  value of st_dev for files on filesystem
            // (4) root:  root of the mount within the filesystem
            // (5) mount point:  mount point relative to the process's root
            // (6) mount options:  per mount options
            // (7) optional fields:  zero or more fields of the form "tag[:value]"
            // (8) separator:  marks the end of the optional fields
            // (9) filesystem type:  name of filesystem of the form "type[.subtype]"
            // (10) mount source:  filesystem specific information or "none"
            // (11) super options:  per super block options
            //
            // Parsers should ignore all unrecognised optional fields.  Currently the
            // possible optional fields are:
            //
            // shared:X  mount is shared in peer group X
            // master:X  mount is slave to peer group X
            // propagate_from:X  mount is slave and receives propagation from peer group X (*)
            // unbindable  mount is unbindable
            ssize_t len;
            char *end, *s;

            while ((len = getline(&line, &line_cap, fd)) >= 0)
            {
                // Create record
                volume_info_t *info = new volume_info_t();
                if (info == NULL)
                    return STATUS_NO_MEM;
                else if (!list.add(info))
                {
                    delete info;
                    return STATUS_NO_MEM;
                }

                // Parse the line
                end         = &line[len];
                // root
                if ((s = move_forward(line, end, 3)) == NULL)
                    return STATUS_BAD_FORMAT;
                if ((res = read_field(&info->root, s, end)) != STATUS_OK)
                    return res;
                // mount point
                if ((s = move_forward(s, end, 1)) == NULL)
                    return STATUS_BAD_FORMAT;
                if ((res = read_field(&info->target, s, end)) != STATUS_OK)
                    return res;
                // end of the optional fields
                if ((s = strstr(s, " - ")) == NULL)
                    return STATUS_BAD_FORMAT;
                s += 3;
                // filesystem type
                if ((res = read_field(&info->name, s, end)) != STATUS_OK)
                    return res;
                // mount source
                if ((s = move_forward(s, end, 1)) == NULL)
                    return STATUS_BAD_FORMAT;
                if ((res = read_field(&info->device, s, end)) != STATUS_OK)
                    return res;

                // Produce final record
                info->flags     = 0;
                if (is_dummy_fs(&info->name, false))
                    info->flags    |= VF_DUMMY;
                if (is_remote_fs(&info->device, &info->name))
                    info->flags    |= VF_REMOTE;
                if (is_posix_drive(&info->device))
                    info->flags    |= VF_DRIVE;
            }

            if (!feof(fd))
                return STATUS_IO_ERROR;

            // Commit and return
            list.swap(volumes);
            return STATUS_OK;
        }

        static status_t read_linux_mntent(const char *path, lltl::parray<volume_info_t> *volumes)
        {
            struct mntent *mnt;
            lltl::parray<volume_info_t> list;
            lsp_finally { free_volume_info(&list); };

            // Open the file
            FILE *fd    = setmntent(path, "r");
            if (fd == NULL)
                return STATUS_NOT_SUPPORTED;
            lsp_finally { endmntent(fd); };

            // Parse records
            while ((mnt = getmntent(fd)) != NULL)
            {
                // Create record
                volume_info_t *info = new volume_info_t();
                if (info == NULL)
                    return STATUS_NO_MEM;
                else if (!list.add(info))
                {
                    delete info;
                    return STATUS_NO_MEM;
                }

                // Fill fields
                bool bind   = hasmntopt (mnt, "bind");
                if (!info->device.set_utf8(mnt->mnt_fsname))
                    return STATUS_NO_MEM;
                if (!info->target.set_utf8(mnt->mnt_dir))
                    return STATUS_NO_MEM;
                if (!info->root.set_ascii("/"))
                    return STATUS_NO_MEM;
                if (!info->name.set_utf8(mnt->mnt_type))
                    return STATUS_NO_MEM;

                // Produce final record
                info->flags     = 0;
                if (is_dummy_fs(&info->name, bind))
                    info->flags    |= VF_DUMMY;
                if (is_remote_fs(&info->device, &info->name))
                    info->flags    |= VF_REMOTE;
                if (is_posix_drive(&info->device))
                    info->flags    |= VF_DRIVE;
            }

            // Commit and return
            list.swap(volumes);
            return STATUS_OK;
        }
    #endif /* PLATFORM_LINUX */

    #ifdef PLATFORM_BSD
        static status_t read_bsd_mntinfo(lltl::parray<volume_info_t> *volumes)
        {
            struct statfs *fsp;
            lltl::parray<volume_info_t> list;
            lsp_finally { free_volume_info(&list); };

            // Get mount information
            int entries = getmntinfo (&fsp, MNT_NOWAIT);
            if (entries < 0)
                return STATUS_NOT_SUPPORTED;

            for (int i=0; i < entries; ++fsp, ++i)
            {
                // Create record
                volume_info_t *info = new volume_info_t();
                if (info == NULL)
                    return STATUS_NO_MEM;
                else if (!list.add(info))
                {
                    delete info;
                    return STATUS_NO_MEM;
                }

                // Fill fields
                if (!info->device.set_utf8(fsp->f_mntfromname))
                    return STATUS_NO_MEM;
                if (!info->target.set_utf8(fsp->f_mntonname))
                    return STATUS_NO_MEM;
                if (!info->root.set_ascii("/"))
                    return STATUS_NO_MEM;
                if (!info->name.set_utf8(fsp->f_fstypename))
                    return STATUS_NO_MEM;

                // Produce final record
                info->flags     = 0;
                if (is_dummy_fs(&info->name, false))
                    info->flags    |= VF_DUMMY;
                if (is_remote_fs(&info->device, &info->name))
                    info->flags    |= VF_REMOTE;
                if (is_posix_drive(&info->device))
                    info->flags    |= VF_DRIVE;
            }

            // Commit and return
            list.swap(volumes);
            return STATUS_OK;
        }
    #endif /* PLATFORM_BSD */

    #ifdef PLATFORM_WINDOWS
        status_t read_pathnames(const WCHAR *volume, WCHAR **list, size_t *cap)
        {
            DWORD cap_req = 0;

            // Obtain volume information
            while (true)
            {
                if (GetVolumePathNamesForVolumeNameW(volume, *list, *cap, &cap_req))
                    return STATUS_OK;
                if (GetLastError() != ERROR_MORE_DATA)
                    return STATUS_UNKNOWN_ERR;

                // Allocate the desired chunk of memory
                cap_req     = lsp::align_size(cap_req, 0x20);
                WCHAR *buf  = static_cast<WCHAR *>(realloc(*list, (cap_req + 1) * sizeof(WCHAR)));
                if (buf == NULL)
                    return STATUS_NO_MEM;
                *list       = buf;
                *cap        = cap_req;
            }
        }

        status_t read_windows_mntinfo(lltl::parray<volume_info_t> *volumes)
        {
            status_t res;
            lltl::parray<volume_info_t> list;
            lsp_finally { free_volume_info(&list); };

            // Read the value
            WCHAR *vol_name = static_cast<WCHAR *>(malloc((MAX_PATH + 4) * 3 * sizeof(WCHAR)));
            if (vol_name == NULL)
                return STATUS_NO_MEM;
            lsp_finally { free(vol_name); };
            WCHAR *dev_name = &vol_name[MAX_PATH + 4];
            WCHAR *fs_name  = &dev_name[MAX_PATH + 4];
            HANDLE h        = INVALID_HANDLE_VALUE;

            // Allocate place for the volume names
            size_t pth_cap  = 0;
            WCHAR *pth_name = NULL;
            lsp_finally {
                if (pth_name != NULL)
                    free(pth_name);
            };

            // Start volume scanning
            if ((h = FindFirstVolumeW(vol_name, MAX_PATH + 4)) == INVALID_HANDLE_VALUE)
                return STATUS_NOT_SUPPORTED;
            lsp_finally { FindVolumeClose(h); };

            do
            {
                //  Skip the \\?\ prefix and remove the trailing backslash.
                WCHAR idx   = wcslen(vol_name) - 1;

                if ((vol_name[0] != L'\\') ||
                    (vol_name[1] != L'\\') ||
                    (vol_name[2] != L'?') ||
                    (vol_name[3] != L'\\') ||
                    (vol_name[idx] != L'\\'))
                    continue;

                // Obtain the device name
                vol_name[idx] = '\0';
                DWORD chars = QueryDosDeviceW(&vol_name[4], dev_name, MAX_PATH);
                vol_name[idx] = '\\';
                if (chars == 0)
                    continue;

                // Obtain the list of path names
                if ((res = read_pathnames(vol_name, &pth_name, &pth_cap)) != STATUS_OK)
                    return res;

                // Do simple stuff if path names have been not obtained
                if (*pth_name == '\0')
                {
                    // Create record
                    volume_info_t *info = new volume_info_t();
                    if (info == NULL)
                        return STATUS_NO_MEM;
                    else if (!list.add(info))
                    {
                        delete info;
                        return STATUS_NO_MEM;
                    }

                    // Fill volume name field
                    if (!info->device.set_utf16(vol_name))
                        return STATUS_NO_MEM;
                    if (!info->root.set_utf16(dev_name))
                        return STATUS_NO_MEM;
                    if (!info->target.set_ascii(""))
                        return STATUS_NO_MEM;
                    if (!info->name.set_ascii(""))
                        return STATUS_NO_MEM;

                    // Obtain flags
                    UINT drv_type = GetDriveTypeW(dev_name);
                    switch (drv_type)
                    {
                        case DRIVE_REMOVABLE:
                        case DRIVE_FIXED:
                        case DRIVE_CDROM:
                        case DRIVE_RAMDISK:
                            info->flags     = VF_DRIVE;
                            break;
                        case DRIVE_REMOTE:
                            info->flags     = VF_REMOTE;
                            break;
                        default:
                            info->flags     = 0;
                            break;
                    }
                    continue;
                }

                for (WCHAR *vol_drive = pth_name; *vol_drive != '\0';)
                {
                    size_t len  = wcslen(vol_drive);
                    lsp_finally { vol_drive = &vol_drive[len + 1]; };

                    // Obtain the volume information
                    if (!GetVolumeInformationW(vol_drive, NULL, 0, NULL, NULL, NULL, fs_name, MAX_PATH + 4))
                        fs_name[0] = '\0';

                    // Create record
                    volume_info_t *info = new volume_info_t();
                    if (info == NULL)
                        return STATUS_NO_MEM;
                    else if (!list.add(info))
                    {
                        delete info;
                        return STATUS_NO_MEM;
                    }

                    // Fill volume name field
                    if (!info->device.set_utf16(vol_name))
                        return STATUS_NO_MEM;
                    if (!info->root.set_utf16(dev_name))
                        return STATUS_NO_MEM;
                    if (!info->target.set_utf16(vol_drive))
                        return STATUS_NO_MEM;
                    if (!info->name.set_utf16(fs_name))
                        return STATUS_NO_MEM;

                    // Obtain flags
                    UINT drv_type = GetDriveTypeW(vol_drive);
                    switch (drv_type)
                    {
                        case DRIVE_REMOVABLE:
                        case DRIVE_FIXED:
                        case DRIVE_CDROM:
                        case DRIVE_RAMDISK:
                            info->flags     = VF_DRIVE;
                            break;
                        case DRIVE_REMOTE:
                            info->flags     = VF_REMOTE;
                            break;
                        default:
                            info->flags     = 0;
                            break;
                    }
                }

            } while (FindNextVolumeW(h, vol_name, MAX_PATH));

            // Commit and return
            list.swap(volumes);
            return STATUS_OK;
        }

        status_t read_windows_netinfo(lltl::parray<volume_info_t> *volumes)
        {
            lltl::parray<volume_info_t> list;
            lsp_finally { free_volume_info(&list); };

            // Open the enumeration
            NETRESOURCEW *local = NULL;
            HANDLE hEnum = NULL;
            DWORD dwResult = WNetOpenEnumW(RESOURCE_CONNECTED, RESOURCETYPE_DISK, 0, local, &hEnum);
            if (dwResult != NO_ERROR)
                return STATUS_NOT_SUPPORTED;
            lsp_finally { WNetCloseEnum(hEnum); };

            // Iterate over resources
            DWORD cEntries = -1;
            DWORD cbBuffer = 16384;
            local = static_cast<NETRESOURCEW *>(GlobalAlloc(GPTR, cbBuffer));
            if (local == NULL)
                return STATUS_NO_MEM;
            lsp_finally { GlobalFree(local); };

            // Perform enumeration
            while (true)
            {
                dwResult = WNetEnumResourceW(hEnum, &cEntries, local, &cbBuffer);
                if (dwResult == ERROR_NO_MORE_ITEMS)
                    break;
                if (dwResult == ERROR_MORE_DATA)
                {
                    NETRESOURCEW *pnew  = static_cast<NETRESOURCEW *>(GlobalReAlloc(local, cbBuffer, GPTR));
                    if (pnew == NULL)
                        return STATUS_NO_MEM;
                    local               = pnew;
                    continue;
                }
                if (dwResult != NO_ERROR)
                    return STATUS_UNKNOWN_ERR;

                // Process each entry
                for (DWORD i = 0; i < cEntries; i++)
                {
                    NETRESOURCEW *item = &local[i];

                    if (item->dwScope != RESOURCE_CONNECTED)
                        continue;
                    if (item->dwType != RESOURCETYPE_DISK)
                        continue;
                    if ((item->dwDisplayType != RESOURCEDISPLAYTYPE_SHARE) &&
                        (item->dwDisplayType != RESOURCEDISPLAYTYPE_DIRECTORY))
                        continue;

                    // Create record
                    volume_info_t *info = new volume_info_t();
                    if (info == NULL)
                        return STATUS_NO_MEM;
                    else if (!list.add(info))
                    {
                        delete info;
                        return STATUS_NO_MEM;
                    }

                    // Fill volume name field
                    if (!info->device.set_utf16(item->lpProvider))
                        return STATUS_NO_MEM;
                    if (!info->root.set_utf16(item->lpRemoteName))
                        return STATUS_NO_MEM;
                    if (!info->target.set_utf16(item->lpLocalName))
                        return STATUS_NO_MEM;
                    if (!info->target.ends_with(FILE_SEPARATOR_C))
                    {
                        if (!info->target.append(FILE_SEPARATOR_C))
                            return STATUS_NO_MEM;
                    }
                    if ((item->lpComment != NULL) && (!info->name.set_utf16(item->lpComment)))
                        return STATUS_NO_MEM;

                    info->flags     = VF_REMOTE;
                }
            }

            // Commit and return
            if (!volumes->add(&list))
                return STATUS_NO_MEM;

            list.flush();
            return STATUS_OK;
        }
    #endif /* PLATFORM_WINDOWS */

        status_t get_volume_info(lltl::parray<volume_info_t> *volumes)
        {
            // Verify input
            if (volumes == NULL)
                return STATUS_BAD_ARGUMENTS;

            status_t res = STATUS_NOT_IMPLEMENTED;
        #ifdef PLATFORM_LINUX
            if ((res = read_linux_mountinfo(volumes)) != STATUS_NOT_SUPPORTED)
                return res;
            if ((res = read_linux_mntent("/proc/self/mounts", volumes)) != STATUS_NOT_SUPPORTED)
                return res;
            if ((res = read_linux_mntent("/proc/mounts", volumes)) != STATUS_NOT_SUPPORTED)
                return res;
            if ((res = read_linux_mntent("/etc/mtab", volumes)) != STATUS_NOT_SUPPORTED)
                return res;
        #endif /* PLATFORM_LINUX */
        #ifdef PLATFORM_BSD
            if ((res = read_bsd_mntinfo(volumes)) != STATUS_NOT_SUPPORTED)
                return res;
        #endif /* PLATFORM_BSD */
        #ifdef PLATFORM_WINDOWS
            res = read_windows_mntinfo(volumes);
            if (res == STATUS_OK)
                res = read_windows_netinfo(volumes);
        #endif /* PLATFORM_WINDOWS */

            return res;
        }

        void free_volume_info(lltl::parray<volume_info_t> *volumes)
        {
            if (volumes == NULL)
                return;

            for (size_t i=0, n=volumes->size(); i<n; ++i)
            {
                volume_info_t *p = volumes->uget(i);
                if (p != NULL)
                    delete p;
            }
            volumes->flush();
        }

        size_t page_size()
        {
        #ifdef PLATFORM_WINDOWS
            SYSTEM_INFO     os_sysinfo;
            GetSystemInfo(&os_sysinfo);

            return os_sysinfo.dwPageSize;
        #else
            return sysconf(_SC_PAGESIZE);
        #endif /* PLATFORM_WINDOWS */
        }

        size_t system_cores()
        {
        #ifdef PLATFORM_WINDOWS
            SYSTEM_INFO     os_sysinfo;
            GetSystemInfo(&os_sysinfo);

            return os_sysinfo.dwNumberOfProcessors;;
        #else
            return sysconf(_SC_NPROCESSORS_ONLN);
        #endif /* PLATFORM_WINDOWS */
        }

    } /* namespace system */
} /* namespace lsp */


