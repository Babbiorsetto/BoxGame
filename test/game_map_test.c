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

    game_map_setPlayer(map, x, y);
    munit_assert_int(game_map_hasPlayer(map, x, y), ==, 1);
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

MunitTest game_map_tests[] =
{
    {"/setPlayer", game_map_setPlayer_test, game_map_test_setup, game_map_test_teardown, MUNIT_SUITE_OPTION_NONE, NULL},
    {"/setObstacle", game_map_setObstacle_test, game_map_test_setup, game_map_test_teardown, MUNIT_SUITE_OPTION_NONE, NULL},
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
