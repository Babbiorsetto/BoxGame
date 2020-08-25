#include "munit.h"
#include "game_map_test.h"

int main(int argc, char *const argv[])
{
    MunitSuite all_suites[] = {game_map_suite, NULL};

    MunitSuite test_suite =
    {
        "",
        NULL,
        all_suites,
        1,
        MUNIT_SUITE_OPTION_NONE
    };

    return munit_suite_main(&test_suite, NULL, argc, argv);
}

