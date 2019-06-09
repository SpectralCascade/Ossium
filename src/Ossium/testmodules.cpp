#include "testmodules.h"

namespace Ossium
{
#ifdef UNIT_TESTS
    namespace Test
    {

        UnitTest::UnitTest()
        {
            passed_test = true;
            total_test_modules++;
            total_passed_test_modules++;
        }

        int UnitTest::total_test_modules = 0;
        int UnitTest::total_passed_test_modules = 0;
        int UnitTest::total_test_asserts = 0;
        int UnitTest::total_passed_test_asserts = 0;
        bool UnitTest::assert_result = true;

    }
#endif
}
