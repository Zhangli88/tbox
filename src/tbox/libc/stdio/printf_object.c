/*!The Treasure Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2017, ruki All rights reserved.
 *
 * @author      ruki
 * @file        printf_object.c
 * @ingroup     libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "printf_object"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../string/string.h"
#include "../../algorithm/algorithm.h"
#include "../../container/container.h"
#include "printf_object.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the printf object entry type
typedef struct __tb_printf_object_entry_t
{
    // the format name
    tb_char_t const*                name;

    // the format func
    tb_printf_object_func_t         func;

}tb_printf_object_entry_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the entry list
static tb_printf_object_entry_t*    g_list = tb_null;

// the entry size
static tb_size_t                    g_size = 0;

// the entry maxn
static tb_size_t                    g_maxn = 16;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_long_t tb_printf_object_comp(tb_iterator_ref_t iterator, tb_cpointer_t item, tb_cpointer_t data)
{
    // the entry
    tb_printf_object_entry_t* entry = (tb_printf_object_entry_t*)item;
    tb_assert(entry && data);

    // comp
    return tb_strcmp(entry->name, (tb_char_t const*)data);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_void_t tb_printf_object_register(tb_char_t const* name, tb_printf_object_func_t func)
{
    // check
    tb_assert_and_check_return(name && g_maxn);
    tb_assert_and_check_return(tb_strlen(name) < TB_PRINTF_OBJECT_NAME_MAXN);

    // init entries
    if (!g_list) g_list = tb_nalloc_type(g_maxn, tb_printf_object_entry_t);
    tb_assert_and_check_return(g_list);

    // full? grow it
    if (g_size >= g_maxn)
    {
        // update maxn
        g_maxn = g_size + 16;

        // resize list
        g_list = (tb_printf_object_entry_t*)tb_ralloc(g_list, g_maxn * sizeof(tb_printf_object_entry_t));
        tb_assert_and_check_return(g_list);
    }

    // find it
    tb_size_t i = 0;
    tb_size_t n = g_size;
    tb_long_t r = -1;
    for (i = 0; i < n; i++) if ((r = tb_strcmp(name, g_list[i].name)) <= 0) break;

    // same? update it
    if (!r) g_list[i].func = func;
    else
    {
        // move it for inserting
        if (i < n) tb_memmov(g_list + i + 1, g_list + i, (n - i) * sizeof(tb_printf_object_entry_t));

        // register it
        g_list[i].name = name;
        g_list[i].func = func;

        // update size
        g_size++;
    }
}
tb_printf_object_func_t tb_printf_object_find(tb_char_t const* name)
{
    // check
    tb_assert_and_check_return_val(g_list && name, tb_null);

    // init the iterator
    tb_array_iterator_t array_iterator;
    tb_iterator_ref_t   iterator = tb_iterator_make_for_mem(&array_iterator, g_list, g_size, sizeof(tb_printf_object_entry_t));
    tb_assert_and_check_return_val(iterator, tb_null);

    // find it
    tb_size_t itor = tb_binary_find_all_if(iterator, tb_printf_object_comp, name);
    tb_check_return_val(itor != tb_iterator_tail(iterator), tb_null);

    // ok?
    return itor < g_size? g_list[itor].func : tb_null;
}
tb_void_t tb_printf_object_exit()
{
    // exit list
    if (g_list) tb_free(g_list);
    g_list = tb_null;

    // exit size
    g_size = 0;
    g_maxn = 0;
}
#ifdef __tb_debug__
tb_void_t tb_printf_object_dump(tb_noarg_t)
{
    // check
    tb_assert_and_check_return(g_list);

    // trace
    tb_trace_i("");

    // done
    tb_size_t i = 0;
    tb_size_t n = g_size;
    for (i = 0; i < n; i++) 
    {
        // trace
        tb_trace_i("format: %s", g_list[i].name);
    }
}
#endif
