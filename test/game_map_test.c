#include "munit.h"
#include "game_map.h"
#include "game_map_test.h"

void *game_map_test_setup(const MunitParameter params[], void *nothing)
{
    struct game_map_t *map;
    const int width = 5, height = 5;

    game_map_create(&map, width, height);

    return map;
}

void game_map_test_teardown(void *data)
{
    struct game_map_t *map = (struct game_map_t *)data;

    game_map_destroy(map);
}

MunitResult game_map_setPlayer_test(const MunitParameter params[], void *data)
{
    struct game_map_t *map = (struct game_map_t *)data;

    const int x = 2, y = 4;

    int error = game_map_setPlayer(map, x, y);
    munit_assert_int(game_map_hasPlayer(map, x, y), ==, 1);
    munit_assert_int(error, ==, 1);
    error = game_map_setPlayer(map, x, y);
    munit_assert_int(error, ==, 2);
    game_map_setObstacle(map, x, 3);
    error = game_map_setPlayer(map, x, 3);
    munit_assert_int(error, ==, 3);
    return MUNIT_OK;
}

MunitResult game_map_setObstacle_test(const MunitParameter params[], void *data)
{
    struct game_map_t *map = (struct game_map_t *)data;

    const int x = 1, y = 2;

    game_map_setObstacle(map, x, y);
    munit_assert_int(game_map_hasObstacle(map, x, y), ==, 1);
    return MUNIT_OK;
}

MunitResult game_map_pickup_test(const MunitParameter params[], void *data)
{
    
    struct game_map_t *map = (struct game_map_t *)data;

    const int x = 3, y = 3, number = 1, duration = 15;
    int retNumber, retDuration;

    game_map_setBox(map, x, y, number, duration);
    int error = game_map_pickup(map, x, y, &retNumber, &retDuration);
    munit_assert_int(error, ==, 1);
    munit_assert_int(retNumber, ==, number);
    munit_assert_int(retDuration, ==, duration);
    error = game_map_pickup(map, x, y, &retNumber, &retDuration);
    munit_assert_int(error, ==, 0);
    return MUNIT_OK;
}

MunitResult game_map_create_test(const MunitParameter params[], void *data)
{
    struct game_map_t *map;
    const int width = 15, height = 3;
    int error = game_map_create(&map, width, height);
    if (error == -1)
    {
        return MUNIT_ERROR;
    }
    munit_assert_not_null(map);
    munit_assert_int(game_map_getWidth(map), ==, width);
    munit_assert_int(game_map_getHeight(map), ==, height);

    game_map_destroy(map);
    return MUNIT_OK;
}

MunitResult game_map_drop_test(const MunitParameter params[], void *data)
{
    struct game_map_t *map = (struct game_map_t *)data;

    const int x = 2, y = 4, number = 3, duration = 8;
    const int goodDropX = 2, goodDropY = 1;
    const int badDropX = 1, badDropY = 1, badNumber = 2;
    const int otherX = 4, otherY = 4;
    int trash, trash2;

    int error = game_map_drop(map, x, y, number, duration);
    munit_assert_int(error, ==, 1);
    munit_assert_int(game_map_pickup(map, x, y, &trash, &trash2), ==, 1);
    munit_assert_int(trash, ==, number);
    munit_assert_int(trash2, ==, duration);

    game_map_setDropoff(map, goodDropX, goodDropY, number);
    error = game_map_drop(map, goodDropX, goodDropY, number, duration);
    munit_assert_int(error, ==, 2);
    munit_assert_int(game_map_pickup(map, goodDropX, goodDropY, &trash, &trash2), ==, 0);

    game_map_setDropoff(map, badDropX, badDropY, badNumber);
    error = game_map_drop(map, badDropX, badDropY, number, duration);
    munit_assert_int(error, ==, 3);
    munit_assert_int(game_map_pickup(map, badDropX, badDropY, &trash, &trash2), ==, 0);

    game_map_drop(map, otherX, otherY, number, duration);
    error = game_map_drop(map, otherX, otherY, 1, 12);
    munit_assert_int(error, ==, 4);
    munit_assert_int(game_map_pickup(map, otherX, otherY, &trash, &trash2), ==, 1);
    munit_assert_int(trash, ==, number);
    munit_assert_int(trash2, ==, duration);

    return MUNIT_OK;
}

MunitTest game_map_tests[] =
{
    {"/create", game_map_create_test, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/setPlayer", game_map_setPlayer_test, game_map_test_setup, game_map_test_teardown, MUNIT_TEST_OPTION_NONE, NULL},
    {"/setObstacle", game_map_setObstacle_test, game_map_test_setup, game_map_test_teardown, MUNIT_TEST_OPTION_NONE, NULL},
    {"/pickup", game_map_pickup_test, game_map_test_setup, game_map_test_teardown, MUNIT_TEST_OPTION_NONE, NULL},
    {"/drop", game_map_drop_test, game_map_test_setup, game_map_test_teardown, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_SUITE_OPTION_NONE, NULL}
};

const MunitSuite game_map_suite =
{
    "/game_map",
    game_map_tests,
    NULL, /* suites */
    1, /* iterations */
    MUNIT_SUITE_OPTION_NONE
};
