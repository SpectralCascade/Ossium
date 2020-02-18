/** COPYRIGHT NOTICE
 *  
 *  Ossium Engine
 *  Copyright (c) 2018-2020 Tim Lane
 *  
 *  This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 *  
 *  Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *  
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *  
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *  
 *  3. This notice may not be removed or altered from any source distribution.
 *  
**/
#include "testmodules.h"

namespace Ossium
{
#ifdef OSSIUM_DEBUG
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
