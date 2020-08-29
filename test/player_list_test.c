#include "munit.h"
#include "player_list.h"
#include "player_list_test.h"

void *player_list_test_setup(const MunitParameter params[], void *nothing)
{
    struct player_list_t *list;
    player_list_create(&list);
    return list;
}

void player_list_test_teardown(void *data)
{
    free(data);
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

MunitResult player_list_iterator_create_test(const MunitParameter params[], void *data)
{
    int error;
    struct player_list_t *list = (struct player_list_t *) data;
    struct player_alias_t *ciccio = (struct player_alias_t *) malloc(sizeof(struct player_alias_t));
    ciccio->username = "Ciccio";
    player_list_add(list, ciccio);
    struct player_list_iterator_t *iterator = NULL;
    error = player_list_iterator_create(list, &iterator);
    munit_assert_int(error, ==, 0);
    munit_assert_not_null(list);

    return MUNIT_OK;
}

MunitResult player_list_iterator_next_test(const MunitParameter params[], void *data)
{
    int last = 0;
    struct player_list_t *list = (struct player_list_t *) data;
    struct player_list_iterator_t *iterator = NULL;
    struct player_alias_t *ciccio = (struct player_alias_t *) malloc(sizeof(struct player_alias_t)) , *nextplayer = NULL;
    struct player_alias_t *pasticho = (struct player_alias_t *) malloc(sizeof(struct player_alias_t));
    ciccio->username = "Ciccio";
    pasticho->username = "Pasticho";
    player_list_add(list,ciccio);
    player_list_iterator_create(list, &iterator);
    nextplayer = player_list_iterator_next(iterator, &last);

    munit_assert_ptr_equal(ciccio, nextplayer);
    munit_assert_int(last, ==, 1);

    player_list_add(list,pasticho);
    nextplayer = player_list_iterator_next(iterator, &last);

    munit_assert_ptr_equal(ciccio, nextplayer);
    munit_assert_int(last, ==, 0);

    nextplayer = player_list_iterator_next(iterator, &last);

    munit_assert_ptr_equal(pasticho, nextplayer);
    munit_assert_int(last, ==, 1);

    free(ciccio);
    free(pasticho);
    player_list_iterator_destroy(iterator);
    
    return MUNIT_OK;
}


/* test array */

MunitTest player_list_tests[] =
{
    {"/create", player_list_create_test, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/iterator_create", player_list_iterator_create_test, player_list_test_setup, player_list_test_teardown, MUNIT_TEST_OPTION_NONE, NULL},
    {"/iterator_next", player_list_iterator_next_test, player_list_test_setup, player_list_test_teardown, MUNIT_TEST_OPTION_NONE, NULL},
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
