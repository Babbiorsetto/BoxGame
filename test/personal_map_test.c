#include "string.h"
#include "munit.h"
#include "game_map.h"
#include "personal_map.h"
#include "personal_map_test.h"

void *personal_map_test_setup(const MunitParameter params[], void *data)
{
    return data;
}

MunitResult personal_map_create_test(const MunitParameter params[], void *data)
{
    struct game_map_t *gameMap;
    struct personal_map_t *map;

    game_map_create(&gameMap, 7, 8);
    game_map_setObstacle(gameMap, 3, 4);
    game_map_setPlayer(gameMap, 1, 2);
    game_map_setDropoff(gameMap, 5, 5, 1);
    game_map_setBox(gameMap, 3, 5, 1, 12);
    /*
    -------
    --P----
    -------
    ----X1-
    -------
    -----A-
    -------
    -------
    */

    personal_map_create(&map, gameMap);
    munit_logf(MUNIT_LOG_DEBUG, "\n%s", personal_map_getString(map));
    munit_assert_int(strlen(personal_map_getString(map)), ==, 63);
    // obstacles should NOT be copied, they are hidden for the player
    munit_assert_char(personal_map_getSymbol(map, 3, 4), ==, '-');
    munit_assert_char(personal_map_getSymbol(map, 1, 2), ==, 'P');
    munit_assert_char(personal_map_getSymbol(map, 5, 5), ==, 'A');
    munit_assert_char(personal_map_getSymbol(map, 3, 5), ==, '1');
    munit_assert_char(personal_map_getSymbol(map, 2, 7), ==, '\n');
    munit_assert_char(personal_map_getSymbol(map, 7, 7), ==, '\0');
    return MUNIT_OK;
}

MunitTest personal_map_tests[] =
{
    {"/create", personal_map_create_test, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

const MunitSuite personal_map_suite =
{
    "/personal_map",
    personal_map_tests,
    NULL, /* suites */
    1, /* iterations */
    MUNIT_SUITE_OPTION_NONE
};
