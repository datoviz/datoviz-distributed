/*************************************************************************************************/
/*  Testing list                                                                                 */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include <stdio.h>

#include "list.h"
#include "test.h"
#include "test_list.h"
#include "testing.h"



/*************************************************************************************************/
/*  List tests                                                                                   */
/*************************************************************************************************/

int test_list_1(TstSuite* suite)
{
    DvzList list = dvz_list();

    int a = 10;
    int b = 20;
    int c = 30;

    // Append and count.
    AT(dvz_list_count(&list) == 0);
    dvz_list_append(&list, a);
    AT(dvz_list_count(&list) == 1);
    dvz_list_append(&list, b);
    AT(dvz_list_count(&list) == 2);
    dvz_list_append(&list, c);
    AT(dvz_list_count(&list) == 3);

    // Get.
    AT(dvz_list_get(&list, 0) == a);
    AT(dvz_list_get(&list, 1) == b);
    AT(dvz_list_get(&list, 2) == c);

    // Indexing.
    AT(dvz_list_index(&list, a) == 0);
    AT(dvz_list_index(&list, b) == 1);
    AT(dvz_list_index(&list, c) == 2);
    AT(dvz_list_index(&list, a + 1) == UINT64_MAX);

    AT(dvz_list_has(&list, a));
    AT(!dvz_list_has(&list, a + 1));

    // Insert
    int d = 40;
    dvz_list_insert(&list, 0, d);

    AT(dvz_list_count(&list) == 4);
    AT(dvz_list_get(&list, 0) == d);
    AT(dvz_list_get(&list, 1) == a);
    AT(dvz_list_get(&list, 2) == b);
    AT(dvz_list_get(&list, 3) == c);

    AT(dvz_list_index(&list, d) == 0);
    AT(dvz_list_index(&list, a) == 1);
    AT(dvz_list_index(&list, b) == 2);
    AT(dvz_list_index(&list, c) == 3);
    AT(dvz_list_index(&list, c + 1) == UINT64_MAX);

    // Remove.
    dvz_list_remove(&list, 1);
    AT(dvz_list_count(&list) == 3);

    AT(dvz_list_get(&list, 0) == d);
    AT(dvz_list_get(&list, 1) == b);
    AT(dvz_list_get(&list, 2) == c);

    AT(dvz_list_index(&list, d) == 0);
    AT(dvz_list_index(&list, b) == 1);
    AT(dvz_list_index(&list, c) == 2);
    AT(dvz_list_index(&list, c + 1) == UINT64_MAX);

    // Destroy the list.
    dvz_list_destroy(&list);
    return 0;
}
