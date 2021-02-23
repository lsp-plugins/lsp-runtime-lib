/*
 * Copyright (C) 2021 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2021 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-runtime-lib
 * Created on: 23 февр. 2021 г.
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
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/fmt/json/dom.h>

UTEST_BEGIN("runtime.fmt.json", dom)

    void create_object(json::Node &node)
    {
        json::Object o = json::Object::build();
        json::Object child = json::Object::build();
        json::Array array = json::Array::build();

        // Create root object
        UTEST_ASSERT(o.set("null", json::Node::build()) == STATUS_OK);
        UTEST_ASSERT(o.set("int", json::Integer::build(42)) == STATUS_OK);
        UTEST_ASSERT(o.set("float", json::Double::build(440.0)) == STATUS_OK);
        UTEST_ASSERT(o.set("bool", json::Boolean::build(true)) == STATUS_OK);
        UTEST_ASSERT(o.set("array", array) == STATUS_OK);
        UTEST_ASSERT(o.set("str", json::String::build("test")) == STATUS_OK);
        UTEST_ASSERT(o.set("object", child) == STATUS_OK);

        // Init array object
        UTEST_ASSERT(array.add(json::Node::build()) == STATUS_OK);
        UTEST_ASSERT(array.add(json::Integer::build(32)) == STATUS_OK);
        UTEST_ASSERT(array.add(json::Double::build(-20.0)) == STATUS_OK);
        UTEST_ASSERT(array.add(json::Boolean::build(true)) == STATUS_OK);
        UTEST_ASSERT(array.add(json::String::build("array")) == STATUS_OK);

        // Init child object
        UTEST_ASSERT(child.set("nv", json::Node::build()) == STATUS_OK);
        UTEST_ASSERT(child.set("iv", json::Integer::build(10)) == STATUS_OK);
        UTEST_ASSERT(child.set("fv", json::Double::build(123.0)) == STATUS_OK);
        UTEST_ASSERT(child.set("bv", json::Boolean::build(false)) == STATUS_OK);
        UTEST_ASSERT(child.set("sv", json::String::build("string")) == STATUS_OK);

        // Return data to the node
        node.assign(o);
    }

    void validate_object(json::Node &node)
    {
        json::Object o = node;

        json::Node nv;
        json::Integer iv;
        json::Double dv;
        json::Boolean bv;
        json::String sv;
        json::Array av;
        json::Object ov;
        LSPString str;

        // Check the object
        UTEST_ASSERT(node.is_object());
        UTEST_ASSERT(o.valid());
        UTEST_ASSERT(o.size() == 7);
        UTEST_ASSERT(o.contains("null"));
        UTEST_ASSERT(o.contains("int"));
        UTEST_ASSERT(o.contains("float"));
        UTEST_ASSERT(o.contains("bool"));
        UTEST_ASSERT(o.contains("str"));
        UTEST_ASSERT(o.contains("array"));
        UTEST_ASSERT(o.contains("object"));

        // Validate object fields
        nv = o.get("null");
        UTEST_ASSERT(nv.valid());
        UTEST_ASSERT(nv.is_null());

        iv = o.get("int");
        UTEST_ASSERT(iv.valid());
        UTEST_ASSERT(iv.is_int());
        UTEST_ASSERT(iv.get() == 42);

        dv = o.get("float");
        UTEST_ASSERT(dv.valid());
        UTEST_ASSERT(dv.is_double());
        UTEST_ASSERT(float_equals_absolute(dv.get(), 440.0));

        bv = o.get("bool");
        UTEST_ASSERT(bv.valid());
        UTEST_ASSERT(bv.is_bool());
        UTEST_ASSERT(bv.get() == true);

        sv = o.get("str");
        UTEST_ASSERT(sv.valid());
        UTEST_ASSERT(sv.is_string());
        UTEST_ASSERT(sv.get(&str) == STATUS_OK);
        UTEST_ASSERT(str.equals_ascii("test"));

        av = o.get("array");
        UTEST_ASSERT(av.valid());
        UTEST_ASSERT(av.is_array());
        UTEST_ASSERT(av.size() == 5);
        {
            nv = av.get(0);
            UTEST_ASSERT(nv.valid());
            UTEST_ASSERT(nv.is_null());

            iv = av.get(1);
            UTEST_ASSERT(iv.valid());
            UTEST_ASSERT(iv.is_int());
            UTEST_ASSERT(iv.get() == 32);

            dv = av.get(2);
            UTEST_ASSERT(dv.valid());
            UTEST_ASSERT(dv.is_double());
            UTEST_ASSERT(float_equals_absolute(dv.get(), -20.0));

            bv = av.get(3);
            UTEST_ASSERT(bv.valid());
            UTEST_ASSERT(bv.is_bool());
            UTEST_ASSERT(bv.get() == true);

            sv = av.get(4);
            UTEST_ASSERT(sv.valid());
            UTEST_ASSERT(sv.is_string());
            UTEST_ASSERT(sv.get(&str) == STATUS_OK);
            UTEST_ASSERT(str.equals_ascii("array"));
        }

        ov = o.get("object");
        UTEST_ASSERT(ov.valid());
        UTEST_ASSERT(ov.is_object());
        UTEST_ASSERT(ov.size() == 5);
        {
            nv = ov.get("nv");
            UTEST_ASSERT(nv.valid());
            UTEST_ASSERT(nv.is_null());

            iv = ov.get("iv");
            UTEST_ASSERT(iv.valid());
            UTEST_ASSERT(iv.is_int());
            UTEST_ASSERT(iv.get() == 10);

            dv = ov.get("fv");
            UTEST_ASSERT(dv.valid());
            UTEST_ASSERT(dv.is_double());
            UTEST_ASSERT(float_equals_absolute(dv.get(), 123.0));

            bv = ov.get("bv");
            UTEST_ASSERT(bv.valid());
            UTEST_ASSERT(bv.is_bool());
            UTEST_ASSERT(bv.get() == false);

            sv = ov.get("sv");
            UTEST_ASSERT(sv.valid());
            UTEST_ASSERT(sv.is_string());
            UTEST_ASSERT(sv.get(&str) == STATUS_OK);
            UTEST_ASSERT(str.equals_ascii("string"));
        }
    }

    void test_create_object()
    {
        printf("Testing JSON object creation");

        json::Node node;
        create_object(node);
        validate_object(node);
    }

    void test_change_object()
    {
        printf("Testing JSON object modification");

        LSPString str;

        json::Object ov = json::Object::build();
        json::Node nv;
        json::Integer iv;
        json::Double dv;
        json::Boolean bv;
        json::String sv;

        UTEST_ASSERT(ov.set("k1", json::Node::build()) == STATUS_OK);
        UTEST_ASSERT(ov.set("k2", json::Integer::build(42)) == STATUS_OK);
        UTEST_ASSERT(ov.set("k3", json::Double::build(440.0)) == STATUS_OK);
        UTEST_ASSERT(ov.set("k4", json::Boolean::build(true)) == STATUS_OK);
        UTEST_ASSERT(ov.set("k5", json::String::build("test")) == STATUS_OK);

        // Check values
        nv = ov.get("k1");
        UTEST_ASSERT(nv.valid());
        UTEST_ASSERT(nv.is_null());

        iv = ov.get("k2");
        UTEST_ASSERT(iv.valid());
        UTEST_ASSERT(iv.is_int());
        UTEST_ASSERT(iv.get() == 42);

        dv = ov.get("k3");
        UTEST_ASSERT(dv.valid());
        UTEST_ASSERT(dv.is_double());
        UTEST_ASSERT(float_equals_absolute(dv.get(), 440.0));

        bv = ov.get("k4");
        UTEST_ASSERT(bv.valid());
        UTEST_ASSERT(bv.is_bool());
        UTEST_ASSERT(bv.get() == true);

        sv = ov.get("k5");
        UTEST_ASSERT(sv.valid());
        UTEST_ASSERT(sv.is_string());
        UTEST_ASSERT(sv.get(&str) == STATUS_OK);
        UTEST_ASSERT(str.equals_ascii("test"));

        // Update values
        UTEST_ASSERT(ov.set("k1", json::Integer::build(32)) == STATUS_OK);
        UTEST_ASSERT(ov.set("k2", json::Double::build(48000.0)) == STATUS_OK);
        UTEST_ASSERT(ov.set("k3", json::Boolean::build(false)) == STATUS_OK);
        UTEST_ASSERT(ov.set("k4", json::String::build("updated")) == STATUS_OK);
        UTEST_ASSERT(ov.set("k5", json::Node::build()) == STATUS_OK);

        // Check values
        iv = ov.get("k1");
        UTEST_ASSERT(iv.valid());
        UTEST_ASSERT(iv.is_int());
        UTEST_ASSERT(iv.get() == 32);

        dv = ov.get("k2");
        UTEST_ASSERT(dv.valid());
        UTEST_ASSERT(dv.is_double());
        UTEST_ASSERT(float_equals_absolute(dv.get(), 48000.0));

        bv = ov.get("k3");
        UTEST_ASSERT(bv.valid());
        UTEST_ASSERT(bv.is_bool());
        UTEST_ASSERT(bv.get() == false);

        sv = ov.get("k4");
        UTEST_ASSERT(sv.valid());
        UTEST_ASSERT(sv.is_string());
        UTEST_ASSERT(sv.get(&str) == STATUS_OK);
        UTEST_ASSERT(str.equals_ascii("updated"));

        nv = ov.get("k5");
        UTEST_ASSERT(nv.valid());
        UTEST_ASSERT(nv.is_null());

        // Overwrite values by reference
        dv = ov.get("k1");
        UTEST_ASSERT(!dv.valid());
        UTEST_ASSERT(!dv.is_double());
        UTEST_ASSERT(dv.set(123.0) == STATUS_OK);

        bv = ov.get("k2");
        UTEST_ASSERT(!bv.valid());
        UTEST_ASSERT(!bv.is_bool());
        UTEST_ASSERT(bv.set(true) == STATUS_OK);

        sv = ov.get("k3");
        UTEST_ASSERT(!sv.valid());
        UTEST_ASSERT(!sv.is_string());
        UTEST_ASSERT(sv.set("reference") == STATUS_OK);

        nv = ov.get("k4");
        UTEST_ASSERT(!nv.is_null());
        nv.set_null();

        iv = ov.get("k5");
        UTEST_ASSERT(!iv.valid());
        UTEST_ASSERT(!iv.is_int());
        UTEST_ASSERT(iv.set(100500) == STATUS_OK);

        // Validate values
        dv = ov.get("k1");
        UTEST_ASSERT(dv.valid());
        UTEST_ASSERT(dv.is_double());
        UTEST_ASSERT(float_equals_absolute(dv.get(), 123.0));

        bv = ov.get("k2");
        UTEST_ASSERT(bv.valid());
        UTEST_ASSERT(bv.is_bool());
        UTEST_ASSERT(bv.get() == true);

        sv = ov.get("k3");
        UTEST_ASSERT(sv.valid());
        UTEST_ASSERT(sv.is_string());
        UTEST_ASSERT(sv.get(&str) == STATUS_OK);
        UTEST_ASSERT(str.equals_ascii("reference"));

        nv = ov.get("k4");
        UTEST_ASSERT(nv.valid());
        UTEST_ASSERT(nv.is_null());

        iv = ov.get("k5");
        UTEST_ASSERT(iv.valid());
        UTEST_ASSERT(iv.is_int());
        UTEST_ASSERT(iv.get() == 100500);
    }

    UTEST_MAIN
    {
        test_create_object();
        test_change_object();
    }
UTEST_END


