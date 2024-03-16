/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 9 февр. 2019 г.
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

#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/io/Path.h>
#include <lsp-plug.in/stdlib/string.h>

#ifdef PLATFORM_WINDOWS
    #define TEST_PATH1          "C:\\Windows\\system32"
    #define TEST_PATH2          "C:\\Windows\\system\\lib"
    #define TEST_PATH2_FIRST    "C:\\Windows\\system"
    #define TEST_PATH2_LAST     "lib"
    #define TEST_PATH3          "C:\\Windows\\system\\bin"
    #define TEST_PATH3_FIRST    "C:\\"
    #define TEST_PATH3_LAST     "Windows\\system\\bin"
    #define TEST_PATH4          "C:\\Windows\\system"
    #define TEST_PATH4_LAST     "system"
    #define TEST_PATH5          "C:\\Windows"
    #define TEST_ROOT           "C:\\"
#else
    #define TEST_PATH1          "/usr/local/bin"
    #define TEST_PATH2          "/usr/share/local/lib"
    #define TEST_PATH2_FIRST    "/usr/share/local"
    #define TEST_PATH2_LAST     "lib"
    #define TEST_PATH3          "/usr/share/local/bin"
    #define TEST_PATH3_FIRST    "/"
    #define TEST_PATH3_LAST     "usr/share/local/bin"
    #define TEST_PATH4          "/usr/share/local"
    #define TEST_PATH4_LAST     "local"
    #define TEST_PATH5          "/usr"
    #define TEST_ROOT           "/"
#endif

using namespace lsp;
using namespace lsp::io;

static char *cnull = NULL;
static LSPString *snull = NULL;
static Path *pnull = NULL;

static const char *cpaths[] =
{
#if !defined(PLATFORM_WINDOWS)
    ".", "",
    "./", "",
    "..", "",
    "../", "",
    "./.././a", "a",
    "a/b/c/../../d/./", "a/d",
    "a///b/c/.//../../d/./", "a/d",
    "a/b/c/../../../d/e/./f/", "d/e/f",

    "//", "/",
    "/.", "/",
    "/..", "/",
    "/./", "/",
    "/../", "/",
    "/a/b/c/../../d/./", "/a/d",
    "/a///b/c/.//../../d/./", "/a/d",
    "/a/b/c/../../../d/e/./f/", "/d/e/f",
    "/../a/b/c/../../d/./", "/a/d",
    "/../a///b/c/.//../../d/./", "/a/d",
    "/./../a/b/c/../../../d/e/./f/", "/d/e/f",
#else
    ".", "",
    ".\\", "",
    "..", "",
    "..\\", "",
    ".\\..\\.\\a", "a",
    "a\\b\\c\\..\\..\\d\\.\\", "a\\d",
    "a\\\\\\b\\c\\.\\\\..\\..\\d\\.\\", "a\\d",
    "a\\b\\c\\..\\..\\..\\d\\e\\.\\f\\", "d\\e\\f",

    "C:\\\\", "C:\\",
    "C:\\.", "C:\\",
    "C:\\..", "C:\\",
    "C:\\.\\", "C:\\",
    "C:\\..\\", "C:\\",
    "C:\\a\\b\\c\\..\\..\\d\\.\\", "C:\\a\\d",
    "C:\\a\\\\\\b\\c\\.\\\\..\\..\\d\\.\\", "C:\\a\\d",
    "C:\\a\\b\\c\\..\\..\\..\\d\\e\\.\\f\\", "C:\\d\\e\\f",
    "C:\\..\\a\\b\\c\\..\\..\\d\\.\\", "C:\\a\\d",
    "C:\\..\\a\\\\\\b\\c\\.\\\\..\\..\\d\\.\\", "C:\\a\\d",
    "C:\\.\\..\\a\\b\\c\\..\\..\\..\\d\\e\\.\\f\\", "C:\\d\\e\\f",
#endif

    NULL, NULL
};

UTEST_BEGIN("runtime.io", path)

//    status_t    get_parent(char *path, size_t maxlen) const;
//    status_t    set_parent(const char *path);
//    status_t    get_parent(LSPString *path) const;
//    status_t    get_parent(Path *path) const;
//    status_t    set_parent(LSPString *path);
//    status_t    set_parent(Path *path);
    void test_get_set_parent()
    {
        Path bp, dp;
        char path[PATH_MAX];
        LSPString spath, dpath;

        printf("Testing get_parent and set_parent...\n");

        UTEST_ASSERT(bp.set(TEST_PATH2) == STATUS_OK);
        UTEST_ASSERT(bp.get_parent(path, PATH_MAX) == STATUS_OK);
        UTEST_ASSERT(bp.get_parent(path, 2) == STATUS_TOO_BIG);
        UTEST_ASSERT(bp.get_parent(cnull, PATH_MAX) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(strcmp(path, TEST_PATH4) == 0);

        UTEST_ASSERT(spath.set_utf8(TEST_PATH4));
        UTEST_ASSERT(bp.get_parent(snull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(bp.get_parent(&dpath) == STATUS_OK);
        UTEST_ASSERT(dpath.equals(&spath));
        UTEST_ASSERT(bp.get_parent(&dp) == STATUS_OK);
        UTEST_ASSERT(dp.equals(&spath));

        UTEST_ASSERT(bp.set(TEST_ROOT) == STATUS_OK);
        UTEST_ASSERT(bp.get_parent(path, PATH_MAX) == STATUS_NOT_FOUND);
        UTEST_ASSERT(bp.get_parent(&dpath) == STATUS_NOT_FOUND);
        UTEST_ASSERT(bp.get_parent(&dp) == STATUS_NOT_FOUND);

        UTEST_ASSERT(bp.set("bin") == STATUS_OK);
        UTEST_ASSERT(bp.get_parent(path, PATH_MAX) == STATUS_NOT_FOUND);
        UTEST_ASSERT(bp.get_parent(&dpath) == STATUS_NOT_FOUND);
        UTEST_ASSERT(bp.get_parent(&dp) == STATUS_NOT_FOUND);
    }

//    status_t    set_last(const char *path);
//    status_t    get_last(char *path, size_t maxlen) const;
//    status_t    set_last(const LSPString *path);
//    status_t    get_last(LSPString *path) const;
//    status_t    set_last(const Path *path);
//    status_t    get_last(Path *path) const;
    void test_get_set_last()
    {
        Path bp, dp, p;
        char path[PATH_MAX];
        LSPString spath, dpath, t1;

        printf("Testing get_last and set_last...\n");

        UTEST_ASSERT(dp.set(TEST_PATH2) == STATUS_OK);
        UTEST_ASSERT(dp.set_last("bin") == STATUS_OK);
        UTEST_ASSERT(dp.set_last(cnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(dp.equals(TEST_PATH3));
        UTEST_ASSERT(dp.get_last(path, PATH_MAX) == STATUS_OK);
        UTEST_ASSERT(strcmp(path, "bin") == 0);
        UTEST_ASSERT(dp.set_last("") == STATUS_OK);
        UTEST_ASSERT(dp.equals(TEST_PATH4));

        UTEST_ASSERT(bp.set_last("bin") == STATUS_OK);
        UTEST_ASSERT(bp.set_last(cnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(bp.equals("bin"));
        UTEST_ASSERT(bp.get_last(cnull, PATH_MAX) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(bp.get_last(path, PATH_MAX) == STATUS_OK);
        UTEST_ASSERT(strcmp(path, "bin") == 0);
        UTEST_ASSERT(bp.set_last("") == STATUS_OK);
        UTEST_ASSERT(bp.is_empty());

        UTEST_ASSERT(dpath.set_utf8("bin"));
        dp.clear();
        UTEST_ASSERT(spath.set_utf8(TEST_PATH2));
        UTEST_ASSERT(bp.set(&spath) == STATUS_OK);
        UTEST_ASSERT(bp.set_last(&dpath) == STATUS_OK);
        UTEST_ASSERT(bp.equals(TEST_PATH3));
        UTEST_ASSERT(bp.get_last(&t1) == STATUS_OK);
        UTEST_ASSERT(bp.get_last(snull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(t1.equals(&dpath));

        UTEST_ASSERT(dp.set_last(&dpath) == STATUS_OK);
        UTEST_ASSERT(dp.equals("bin"));
        UTEST_ASSERT(dp.get_last(&spath) == STATUS_OK);
        UTEST_ASSERT(t1.equals(&spath));

        UTEST_ASSERT(bp.set(TEST_PATH2) == STATUS_OK);
        UTEST_ASSERT(dp.set("bin") == STATUS_OK);
        UTEST_ASSERT(bp.set_last(&dp) == STATUS_OK);
        UTEST_ASSERT(bp.set_last(pnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(bp.equals(TEST_PATH3));
        UTEST_ASSERT(bp.get_last(&p) == STATUS_OK);
        UTEST_ASSERT(p.equals("bin"));
        UTEST_ASSERT(bp.set(TEST_ROOT) == STATUS_OK);
        UTEST_ASSERT(bp.get_last(&p) == STATUS_OK);
        UTEST_ASSERT(p.is_empty());
    }

//    status_t    set(const char *path);
//    status_t    set(const LSPString *path);
//    status_t    set(const Path *path);
//    status_t    get(char *path, size_t maxlen) const;
//    status_t    get(LSPString *path) const;
//    status_t    get(Path *path) const;
    void test_get_set()
    {
        Path p, dp, bp;
        char path[PATH_MAX];
        LSPString spath, dpath;

        printf("Testing get and set...\n");

        UTEST_ASSERT(p.set(TEST_PATH1) == STATUS_OK);
        UTEST_ASSERT(p.set(cnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.get(cnull, PATH_MAX) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.get(path, PATH_MAX) == STATUS_OK);
        UTEST_ASSERT(p.get(path, 10) == STATUS_TOO_BIG);
        UTEST_ASSERT(strcmp(path, TEST_PATH1) == 0);

        UTEST_ASSERT(spath.set_utf8(TEST_PATH2));
        UTEST_ASSERT(p.set(&spath) == STATUS_OK);
        UTEST_ASSERT(p.set(snull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.get(snull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.get(&dpath) == STATUS_OK);
        UTEST_ASSERT(dpath.equals(&spath));


        UTEST_ASSERT(dp.set(&p) == STATUS_OK);
        UTEST_ASSERT(dp.set(pnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(dp.get(&bp) == STATUS_OK);
        UTEST_ASSERT(dp.get(pnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(bp.equals(&dp));
        UTEST_ASSERT(bp.equals(&p));
        UTEST_ASSERT(bp.equals(TEST_PATH2));
        UTEST_ASSERT(bp.equals(&spath));
    }

//    status_t    concat(const char *path);
//    status_t    concat(LSPString *path);
//    status_t    concat(Path *path);
    void test_concat()
    {
        Path p;
        LSPString sstr;
        Path spath;

        printf("Testing concat...\n");

        UTEST_ASSERT(p.set(TEST_PATH4) == STATUS_OK);
        UTEST_ASSERT(p.concat(FILE_SEPARATOR_S "bin") == STATUS_OK);
        UTEST_ASSERT(p.concat(cnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals(TEST_PATH3));

        p.clear();
        UTEST_ASSERT(p.concat(TEST_ROOT) == STATUS_OK);
        UTEST_ASSERT(p.is_root());

        p.clear();
        UTEST_ASSERT(p.set(TEST_PATH4) == STATUS_OK);
        UTEST_ASSERT(sstr.set_utf8(FILE_SEPARATOR_S "bin"));
        UTEST_ASSERT(p.concat(&sstr) == STATUS_OK);
        UTEST_ASSERT(p.concat(snull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals(TEST_PATH3));

        p.clear();
        UTEST_ASSERT(p.set(TEST_PATH4) == STATUS_OK);
        UTEST_ASSERT(spath.set(&sstr) == STATUS_OK);
        UTEST_ASSERT(p.concat(&spath) == STATUS_OK);
        UTEST_ASSERT(p.concat(pnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals(TEST_PATH3));

        p.clear();
        UTEST_ASSERT(sstr.set_utf8(TEST_ROOT));
        UTEST_ASSERT(p.concat(&sstr) == STATUS_OK);
        UTEST_ASSERT(p.is_root());

        p.clear();
        UTEST_ASSERT(spath.set(&sstr) == STATUS_OK);
        UTEST_ASSERT(p.concat(&spath) == STATUS_OK);
        UTEST_ASSERT(p.is_root());
    }

//    status_t    append_child(const char *path);
//    status_t    append_child(LSPString *path);
//    status_t    append_child(Path *path);
    void test_append_child()
    {
        Path p;
        LSPString sstr;
        Path spath;

        printf("Testing append_child...\n");

        UTEST_ASSERT(p.set(TEST_PATH4) == STATUS_OK);
        UTEST_ASSERT(p.append_child("bin") == STATUS_OK);
        UTEST_ASSERT(p.append_child(cnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals(TEST_PATH3));

        p.clear();
        UTEST_ASSERT(sstr.set_utf8("bin"));
        UTEST_ASSERT(p.set(TEST_PATH4) == STATUS_OK);
        UTEST_ASSERT(p.append_child(&sstr) == STATUS_OK);
        UTEST_ASSERT(p.append_child(snull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals(TEST_PATH3));

        p.clear();
        UTEST_ASSERT(p.set(TEST_PATH4) == STATUS_OK);
        UTEST_ASSERT(spath.set(&sstr) == STATUS_OK);
        UTEST_ASSERT(p.append_child(&spath) == STATUS_OK);
        UTEST_ASSERT(p.append_child(pnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals(TEST_PATH3));

        UTEST_ASSERT(sstr.set_utf8(TEST_ROOT));
        UTEST_ASSERT(spath.set(&sstr) == STATUS_OK);
        UTEST_ASSERT(p.append_child(TEST_ROOT) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.append_child(&sstr) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.append_child(&spath) == STATUS_INVALID_VALUE);

        sstr.clear();
        spath.clear();
        UTEST_ASSERT(p.append_child("") == STATUS_OK);
        UTEST_ASSERT(p.append_child(&sstr) == STATUS_OK);
        UTEST_ASSERT(p.append_child(&spath) == STATUS_OK);
        UTEST_ASSERT(p.equals(TEST_PATH3));
    }

//    status_t    remove_last();
//    status_t    remove_last(char *path, size_t maxlen);
//    status_t    remove_last(LSPString *path);
//    status_t    remove_last(Path *path);
    void test_remove_last()
    {
        Path p;
        char path[PATH_MAX];
        LSPString sstr, xstr;
        Path spath;

        printf("Testing remove_last...\n");

        UTEST_ASSERT(p.set(TEST_PATH2) == STATUS_OK);
        UTEST_ASSERT(p.remove_last() == STATUS_OK);
        UTEST_ASSERT(p.equals(TEST_PATH4));

        UTEST_ASSERT(p.set(TEST_PATH5) == STATUS_OK);
        UTEST_ASSERT(p.remove_last() == STATUS_OK);
        UTEST_ASSERT(p.equals(TEST_ROOT));

        UTEST_ASSERT(p.set(TEST_ROOT) == STATUS_OK);
        UTEST_ASSERT(p.remove_last() == STATUS_OK);
        UTEST_ASSERT(p.equals(TEST_ROOT));

        UTEST_ASSERT(p.set(TEST_PATH2) == STATUS_OK);
        UTEST_ASSERT(p.remove_last(path, PATH_MAX) == STATUS_OK);
        UTEST_ASSERT(p.remove_last(cnull, PATH_MAX) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.remove_last(path, 3) == STATUS_TOO_BIG);
        UTEST_ASSERT(p.equals(TEST_PATH2_FIRST));
        UTEST_ASSERT(strcmp(path, TEST_PATH2_LAST) == 0);

        UTEST_ASSERT(xstr.set_utf8(TEST_PATH2_LAST));
        UTEST_ASSERT(p.set(TEST_PATH2) == STATUS_OK);
        UTEST_ASSERT(p.remove_last(&sstr) == STATUS_OK);
        UTEST_ASSERT(p.remove_last(snull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals(TEST_PATH2_FIRST));
        UTEST_ASSERT(sstr.equals(&xstr));

        UTEST_ASSERT(p.set(TEST_PATH2) == STATUS_OK);
        UTEST_ASSERT(p.remove_last(&spath) == STATUS_OK);
        UTEST_ASSERT(p.remove_last(pnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals(TEST_PATH2_FIRST));
        UTEST_ASSERT(spath.equals(&xstr));
    }

//    status_t    without_last(char *path, size_t maxlen);
//    status_t    without_last(LSPString *path);
//    status_t    without_last(Path *path);
    void test_without_last()
    {
        Path p;
        char path[PATH_MAX];
        LSPString sstr, xstr;
        Path spath;

        printf("Testing without_last...\n");

        UTEST_ASSERT(p.set(TEST_PATH2) == STATUS_OK);
        UTEST_ASSERT(p.without_last(path, PATH_MAX) == STATUS_OK);
        UTEST_ASSERT(p.without_last(cnull, PATH_MAX) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.without_last(path, 3) == STATUS_TOO_BIG);
        UTEST_ASSERT(p.equals(TEST_PATH2));
        UTEST_ASSERT(strcmp(path, TEST_PATH4) == 0);

        UTEST_ASSERT(xstr.set_utf8(TEST_PATH4));
        UTEST_ASSERT(p.set(TEST_PATH2) == STATUS_OK);
        UTEST_ASSERT(p.without_last(&sstr) == STATUS_OK);
        UTEST_ASSERT(p.without_last(snull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals(TEST_PATH2));
        UTEST_ASSERT(sstr.equals(&xstr));

        UTEST_ASSERT(p.set(TEST_PATH2) == STATUS_OK);
        UTEST_ASSERT(p.without_last(&spath) == STATUS_OK);
        UTEST_ASSERT(p.without_last(pnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals(TEST_PATH2));
        UTEST_ASSERT(spath.equals(&xstr));
    }

    //    status_t    remove_first();
    //    status_t    remove_first(char *path, size_t maxlen);
    //    status_t    remove_first(LSPString *path);
    //    status_t    remove_first(Path *path);
    void test_remove_first()
    {
        Path p;
        char path[PATH_MAX];
        LSPString sstr, xstr;
        Path spath;

        printf("Testing remove_first...\n");

        UTEST_ASSERT(p.set(TEST_PATH3) == STATUS_OK);
        UTEST_ASSERT(p.remove_first() == STATUS_OK);
        UTEST_ASSERT(p.equals(TEST_PATH3_LAST));

        UTEST_ASSERT(p.set(TEST_ROOT) == STATUS_OK);
        UTEST_ASSERT(p.remove_first() == STATUS_NOT_FOUND);
        UTEST_ASSERT(p.equals(TEST_ROOT));

        UTEST_ASSERT(p.set(TEST_PATH3) == STATUS_OK);
        UTEST_ASSERT(p.remove_first(path, PATH_MAX) == STATUS_OK);
        UTEST_ASSERT(p.remove_first(cnull, PATH_MAX) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.remove_first(path, 3) == STATUS_TOO_BIG);
        UTEST_ASSERT(p.equals(TEST_PATH3_LAST));
        UTEST_ASSERT(strcmp(path, TEST_PATH3_FIRST) == 0);

        UTEST_ASSERT(xstr.set_utf8(TEST_PATH3_FIRST));
        UTEST_ASSERT(p.set(TEST_PATH3) == STATUS_OK);
        UTEST_ASSERT(p.remove_first(&sstr) == STATUS_OK);
        UTEST_ASSERT(p.remove_first(snull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals(TEST_PATH3_LAST));
        UTEST_ASSERT(sstr.equals(&xstr));

        UTEST_ASSERT(p.set(TEST_PATH3) == STATUS_OK);
        UTEST_ASSERT(p.remove_first(&spath) == STATUS_OK);
        UTEST_ASSERT(p.remove_first(pnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals(TEST_PATH3_LAST));
        UTEST_ASSERT(spath.equals(&xstr));
    }

    //    status_t    without_first(char *path, size_t maxlen);
    //    status_t    without_first(LSPString *path);
    //    status_t    without_first(Path *path);
    void test_without_first()
    {
        Path p;
        char path[PATH_MAX];
        LSPString sstr, xstr;
        Path spath;

        printf("Testing without_first...\n");

        UTEST_ASSERT(p.set(TEST_PATH3) == STATUS_OK);
        UTEST_ASSERT(p.without_first(path, PATH_MAX) == STATUS_OK);
        UTEST_ASSERT(p.without_first(cnull, PATH_MAX) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.without_first(path, 1) == STATUS_TOO_BIG);
        UTEST_ASSERT(p.equals(TEST_PATH3));
        UTEST_ASSERT(strcmp(path, TEST_PATH3_LAST) == 0);

        UTEST_ASSERT(xstr.set_utf8(TEST_PATH3_LAST));
        UTEST_ASSERT(p.set(TEST_PATH3) == STATUS_OK);
        UTEST_ASSERT(p.without_first(&sstr) == STATUS_OK);
        UTEST_ASSERT(p.without_first(snull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals(TEST_PATH3));
        UTEST_ASSERT(sstr.equals(&xstr));

        UTEST_ASSERT(p.set(TEST_PATH3) == STATUS_OK);
        UTEST_ASSERT(p.without_first(&spath) == STATUS_OK);
        UTEST_ASSERT(p.without_first(pnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals(TEST_PATH3));
        UTEST_ASSERT(spath.equals(&xstr));
    }

//    status_t    remove_base(const char *path);
//    status_t    remove_base(const LSPString *path);
//    status_t    remove_base(const Path *path);
    void test_remove_base()
    {
        Path p, xp;
        LSPString xs;

        printf("Testing remove_base...\n");

        UTEST_ASSERT(p.set(TEST_PATH3) == STATUS_OK);
        UTEST_ASSERT(p.remove_base(TEST_PATH4) == STATUS_OK);
        UTEST_ASSERT(p.remove_base(cnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals("bin"));

        UTEST_ASSERT(p.set(TEST_PATH3) == STATUS_OK);
        UTEST_ASSERT(p.remove_base(TEST_PATH3) == STATUS_OK);
        UTEST_ASSERT(p.is_empty());

        UTEST_ASSERT(xs.set_utf8(TEST_PATH4));
        UTEST_ASSERT(p.set(TEST_PATH3) == STATUS_OK);
        UTEST_ASSERT(p.remove_base(&xs) == STATUS_OK);
        UTEST_ASSERT(p.remove_base(snull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals("bin"));

        UTEST_ASSERT(xs.set_utf8(TEST_PATH3));
        UTEST_ASSERT(p.set(TEST_PATH3) == STATUS_OK);
        UTEST_ASSERT(p.remove_base(&xs) == STATUS_OK);
        UTEST_ASSERT(p.is_empty());

        UTEST_ASSERT(xp.set(TEST_PATH4) == STATUS_OK);
        UTEST_ASSERT(p.set(TEST_PATH3) == STATUS_OK);
        UTEST_ASSERT(p.remove_base(&xp) == STATUS_OK);
        UTEST_ASSERT(p.remove_base(pnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals("bin"));

        UTEST_ASSERT(xp.set(TEST_PATH3) == STATUS_OK);
        UTEST_ASSERT(p.set(TEST_PATH3) == STATUS_OK);
        UTEST_ASSERT(p.remove_base(&xp) == STATUS_OK);
        UTEST_ASSERT(p.is_empty());

        UTEST_ASSERT(p.set(TEST_PATH3) == STATUS_OK);
        UTEST_ASSERT(p.remove_base() == STATUS_OK);
        UTEST_ASSERT(p.equals("bin"));

        UTEST_ASSERT(p.set(TEST_PATH4) == STATUS_OK);
        UTEST_ASSERT(p.remove_base() == STATUS_OK);
        UTEST_ASSERT(p.equals(TEST_PATH4_LAST));
    }

//    void        clear()
//    bool        is_absolute() const;
//    bool        is_relative() const;
//    bool        is_empty() const
//    bool        is_root() const;
//    status_t    root();
    void test_flags()
    {
        Path p;

        printf("Testing flags...\n");

        UTEST_ASSERT(!p.is_absolute());
        UTEST_ASSERT(p.is_relative());
        UTEST_ASSERT(p.is_empty());
        UTEST_ASSERT(!p.is_root());
        UTEST_ASSERT(p.root() == STATUS_BAD_STATE);

        UTEST_ASSERT(p.set(TEST_ROOT) == STATUS_OK);
        UTEST_ASSERT(p.is_absolute());
        UTEST_ASSERT(!p.is_relative());
        UTEST_ASSERT(!p.is_empty());
        UTEST_ASSERT(p.is_root());
        UTEST_ASSERT(p.root() == STATUS_OK);
        UTEST_ASSERT(p.equals(TEST_ROOT));

        UTEST_ASSERT(p.set(TEST_PATH1) == STATUS_OK);
        UTEST_ASSERT(p.is_absolute());
        UTEST_ASSERT(!p.is_relative());
        UTEST_ASSERT(!p.is_empty());
        UTEST_ASSERT(!p.is_root());
        UTEST_ASSERT(p.root() == STATUS_OK);
        UTEST_ASSERT(p.equals(TEST_ROOT));
        UTEST_ASSERT(p.is_root());

        UTEST_ASSERT(p.set("bin") == STATUS_OK);
        UTEST_ASSERT(!p.is_absolute());
        UTEST_ASSERT(p.is_relative());
        UTEST_ASSERT(!p.is_empty());
        UTEST_ASSERT(!p.is_root());
        UTEST_ASSERT(p.root() == STATUS_BAD_STATE);
        UTEST_ASSERT(!p.equals(TEST_ROOT));
        UTEST_ASSERT(!p.is_root());
    }

//    status_t    get_canonical(char *path, size_t maxlen) const;
//    status_t    get_canonical(LSPString *path) const;
//    status_t    get_canonical(Path *path) const;
//    bool        is_canonical() const;
//    status_t    canonicalize();
    void test_canonical()
    {
        Path p, xp;
        LSPString xs;
        char path[PATH_MAX];

        printf("Testing canonical paths...\n");

//        UTEST_ASSERT(p.set(TEST_PATH1) == STATUS_OK);
//        UTEST_ASSERT(p.is_canonical());
//        UTEST_ASSERT(p.set(TEST_ROOT) == STATUS_OK);
//        UTEST_ASSERT(p.is_canonical());
//        p.clear();
//        UTEST_ASSERT(p.is_canonical());

        for (const char **cp=cpaths; *cp != NULL; cp += 2)
        {
            printf("  testing \"%s\" -> \"%s\"\n", cp[0], cp[1]);
            UTEST_ASSERT(p.set(cp[0]) == STATUS_OK);
            UTEST_ASSERT(!p.is_canonical());

            UTEST_ASSERT(p.get_canonical(path, PATH_MAX) == STATUS_OK);
            UTEST_ASSERT(p.get_canonical(cnull, PATH_MAX) == STATUS_BAD_ARGUMENTS);
            UTEST_ASSERT_MSG(strcmp(path, cp[1]) == 0, "canonicalized: \"%s\" -> \"%s\"\n", p.get(), path);
            UTEST_ASSERT(p.equals(cp[0]));

            UTEST_ASSERT(p.get_canonical(&xp) == STATUS_OK);
            UTEST_ASSERT(p.get_canonical(pnull) == STATUS_BAD_ARGUMENTS);
            UTEST_ASSERT_MSG(xp.equals(cp[1]), "canonicalized: \"%s\" -> \"%s\"\n", p.get(), xp.get());
            UTEST_ASSERT(p.equals(cp[0]));

            UTEST_ASSERT(p.get_canonical(&xs) == STATUS_OK);
            UTEST_ASSERT(p.get_canonical(snull) == STATUS_BAD_ARGUMENTS);
            UTEST_ASSERT_MSG(xp.equals(&xs), "canonicalized: \"%s\" -> \"%s\"\n", p.get(), xs.get_utf8());
            UTEST_ASSERT(p.equals(cp[0]));

            UTEST_ASSERT_MSG(p.canonicalize() == STATUS_OK, "canonicalized: \"%s\" -> \"%s\"\n", cp[0], p.get());
            UTEST_ASSERT(p.is_canonical());
            UTEST_ASSERT(p.equals(cp[1]));
        }
    }

    void test_dots()
    {
        struct dot_t
        {
            const char *path;
            bool dot;
            bool dotdot;
        };

        static const dot_t dots[] =
        {
            { "", false, false },
            { ".", true, false },
            { "..", false, true },
            { "...", false, false },
            { FILE_SEPARATOR_S "..", false, true },
            { FILE_SEPARATOR_S ".", true, false },
            { FILE_SEPARATOR_S "...", false, false },
            { FILE_SEPARATOR_S, false, false },
            { "." FILE_SEPARATOR_S "a", false, false },
            { ".." FILE_SEPARATOR_S "a", false, false },
            { "a" FILE_SEPARATOR_S ".", true, false },
            { "a" FILE_SEPARATOR_S "..", false, true },
            { "a" FILE_SEPARATOR_S "...", false, false },
            { NULL, false, false }
        };

        io::Path p;
        LSPString s;

        for (const dot_t *d = dots; d->path != NULL; ++d)
        {
            printf("Testing \"%s\"\n", d->path);
            UTEST_ASSERT(Path::is_dot(d->path) == d->dot);
            UTEST_ASSERT(Path::is_dotdot(d->path) == d->dotdot);
            UTEST_ASSERT(Path::is_dots(d->path) == (d->dot || d->dotdot));

            UTEST_ASSERT(p.set(d->path) == STATUS_OK);
            UTEST_ASSERT(p.is_dot() == d->dot);
            UTEST_ASSERT(p.is_dotdot() == d->dotdot);
            UTEST_ASSERT(p.is_dots() == (d->dot || d->dotdot));

            UTEST_ASSERT(Path::is_dot(&p) == d->dot);
            UTEST_ASSERT(Path::is_dotdot(&p) == d->dotdot);
            UTEST_ASSERT(Path::is_dots(&p) == (d->dot || d->dotdot));

            UTEST_ASSERT(s.set_utf8(d->path));
            UTEST_ASSERT(Path::is_dot(&s) == d->dot);
            UTEST_ASSERT(Path::is_dotdot(&s) == d->dotdot);
            UTEST_ASSERT(Path::is_dots(&s) == (d->dot || d->dotdot));
        }
    }

    void test_relative()
    {
        struct rel_t
        {
            const char *child;
            const char *base;
            status_t code;
            const char *res;
        };

        static const rel_t paths[] =
        {
            { "", "", STATUS_NOT_FOUND, "" },
            { "", "/", STATUS_NOT_FOUND, NULL },
            { "/", "/", STATUS_OK, "" },
            { "a", "a", STATUS_OK, "" },
            { "abc", "a", STATUS_NOT_FOUND, NULL },
            { "a", "abc", STATUS_NOT_FOUND, NULL },
            { "/a", "/a", STATUS_OK, "" },
            { "/a/b", "/a", STATUS_OK, "b" },
            { "/a/b/c", "/a", STATUS_OK, "b/c" },
            { "a", "b", STATUS_NOT_FOUND, NULL },
            { "/a", "/b", STATUS_OK, "../a" },
            { "/b", "/a", STATUS_OK, "../b" },
            { "/a/b/c", "/a/b/d", STATUS_OK, "../c" },
            { "/a/b", "/a/b/d", STATUS_OK, ".." },
            { "/a/", "/a/b/d/", STATUS_OK, "../.." },
            { "/a/x", "/a/b/d/", STATUS_OK, "../../x" },
            { "/a/c/../b", "/a/../a/b/d", STATUS_OK, ".." },

            { NULL, NULL, 0, NULL }
        };

        printf("Testing as_relative() methods...\n");

        for (const rel_t *d = paths; d->child != NULL; ++d)
        {
            if (d->code == STATUS_OK)
                printf("Testing \"%s\" - \"%s\" -> \"%s\" \n", d->child, d->base, d->res);
            else
                printf("Testing \"%s\" - \"%s\" -> error(%d) \n", d->child, d->base, int(d->code));

            io::Path base, child, res;
            UTEST_ASSERT(base.set(d->base) == STATUS_OK);
            UTEST_ASSERT(child.set(d->child) == STATUS_OK);

            status_t code = child.as_relative(&base);
            UTEST_ASSERT_MSG(code == d->code, "Invalid code %d", int(code));
            if (d->code == STATUS_OK)
            {
                UTEST_ASSERT(res.set(d->res) == STATUS_OK);
                UTEST_ASSERT_MSG(child.equals(&res), "Returned path: %s", child.as_utf8());
            }
        }
    }

    void test_ext()
    {
        struct file_t
        {
            const char *path;
            const char *noext;
            const char *ext;
        };

        LSPString spath;
        io::Path ipath;
        char cpath[32];

        static const file_t files[] =
        {
            { "", "", ""},
            { "a", "a", ""},
            { "long_file", "long_file", ""},
            { "file.ext", "file", "ext"},
            { ".config", "", "config"},
            { "file.ext1.ext2", "file.ext1", "ext2"},

            { "/path/a", "a", ""},
            { "/path/long_file", "long_file", ""},
            { "/path/file.ext", "file", "ext"},
            { "/path/.config", "", "config"},
            { "/path/file.ext1.ext2", "file.ext1", "ext2"},

            { NULL, NULL, NULL }
        };

        printf("Testing get_ext() and get_noext() methods...\n");

        for (const file_t *f = files; f->path != NULL; ++f)
        {
            io::Path tmp;
            UTEST_ASSERT(tmp.set(f->path) == STATUS_OK);

            // noext()
            printf("  testing noext('%s') \n", f->path);
            UTEST_ASSERT(tmp.get_last_noext(&spath) == STATUS_OK);
            UTEST_ASSERT(tmp.get_last_noext(&ipath) == STATUS_OK);
            UTEST_ASSERT(tmp.get_last_noext(cpath, sizeof(cpath)) == STATUS_OK);
            UTEST_ASSERT(spath.equals_ascii(f->noext));
            UTEST_ASSERT(ipath.as_string()->equals_ascii(f->noext));
            UTEST_ASSERT(strcmp(cpath, f->noext) == 0);

            // ext()
            printf("  testing ext('%s') \n", f->path);
            UTEST_ASSERT(tmp.get_ext(&spath) == STATUS_OK);
            UTEST_ASSERT(tmp.get_ext(&ipath) == STATUS_OK);
            UTEST_ASSERT(tmp.get_ext(cpath, sizeof(cpath)) == STATUS_OK);
            UTEST_ASSERT(spath.equals_ascii(f->ext));
            UTEST_ASSERT(ipath.as_string()->equals_ascii(f->ext));
            UTEST_ASSERT(strcmp(cpath, f->ext) == 0);
        }
    }

    void test_final_path()
    {
        io::Path tmp, fpath;

        tmp.set("/home/sadko/tmp/symlink-test/a");
        UTEST_ASSERT(tmp.final_path(&fpath) == STATUS_OK);
        UTEST_ASSERT(fpath.equals("/home/sadko/tmp/symlink-test/3/e.txt"));

        tmp.set("/home/sadko/tmp/symlink-test/A");
        UTEST_ASSERT(tmp.final_path(&fpath) == STATUS_OVERFLOW);
    }

    UTEST_MAIN
    {
        test_get_set();
        test_get_set_last();
        test_get_set_parent();
        test_concat();
        test_append_child();
        test_remove_last();
        test_without_last();
        test_remove_first();
        test_without_first();
        test_remove_base();
        test_flags();
        test_canonical();
        test_dots();
        test_relative();
        test_ext();
        test_final_path();
    }
UTEST_END;



