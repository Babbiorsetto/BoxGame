#include "munit.h"
#include "player_list.h"
#include "player_list_test.h"

void *player_list_test_setup(const MunitParameter params[], void *nothing)
{

}

void player_list_test_teardown(void *data)
{

}

/* tests */

MunitResult player_list_create_test(const MunitParameter params[], void *data)
{
    struct player_list_t *list;

    int error = player_list_create(&list);
    munit_assert_int(error, ==, 0);
    munit_assert_int(player_list_is_empty(list), ==, 1);
    player_list_destroy(list);
    
    return MUNIT_OK;
}

/* test array */

MunitTest player_list_tests[] =
{
    {"/create", player_list_create_test, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

const MunitSuite player_list_suite =
{
    "/player_list",
    player_list_tests,
    NULL,
    1,
    MUNIT_SUITE_OPTION_NONE
};
