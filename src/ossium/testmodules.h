#ifndef TESTMODULES_H
#define TESTMODULES_H

#include <string>

#include "circularbuffer.h"

namespace ossium
{

    namespace test
    {

        class UnitTest
        {
        public:
            UnitTest();

            virtual void RunTest() = 0;

            /// Has this module passed the tests or not?
            bool passed_test;

            /// Total test assertions
            static int total_test_asserts;

            /// Total test assertions passed
            static int total_passed_test_asserts;

            /// Total tests altogether
            static int total_test_modules;

            /// Total test modules passed
            static int total_passed_test_modules;

        protected:
            /// Used for evaluating test conditions
            static bool assert_result;

        };

        /// Use this macro to run a unit test
        #define TEST_RUN(MODULE)                                                                                    \
                MODULE MODULE##_test_obj;                                                                            \
                SDL_Log("\n\nRunning unit test '" #MODULE "':");                                                    \
                MODULE##_test_obj.RunTest();                                                                         \
                if (!MODULE##_test_obj.passed_test)                                                                  \
                {                                                                                                   \
                    UnitTest::total_passed_test_modules--;                                                          \
                }                                                                                                   \
                SDL_Log("Test module '" #MODULE "' %s!\n\n", MODULE##_test_obj.passed_test ? "PASSED" : "FAILED")

        /// Use this macro to assert a test condition in a class inheriting from UnitTest
        /// where TEST_CONDITION is the boolean test condition.
        #define TEST_ASSERT(TEST_CONDITION)                                                                     \
                assert_result = TEST_CONDITION;                                                                 \
                total_test_asserts++;                                                                           \
                if (!assert_result)                                                                             \
                {                                                                                               \
                    passed_test = false;                                                                        \
                }                                                                                               \
                else                                                                                            \
                {                                                                                               \
                    total_passed_test_asserts++;                                                                \
                }                                                                                               \
                SDL_Log("[%s] Test condition '" #TEST_CONDITION "'.", assert_result ? "PASSED" : "!FAILED!" );  \
                SDL_assert(assert_result);

        /// When you have finished running all unit tests, use this evaluation macro to log the final results,
        /// pause program execution if any tests failed, and reset the overall unit test results
        #define TEST_EVALUATE()                                                                                     \
                SDL_Log("Test evaluation results: %d of %d modules passed tests successfully.",                     \
                        UnitTest::total_passed_test_modules, UnitTest::total_test_modules);                         \
                SDL_Log("%d of %d test assertions passed successfully.", UnitTest::total_passed_test_asserts,       \
                        UnitTest::total_test_asserts);                                                              \
                SDL_assert(UnitTest::total_passed_test_modules == UnitTest::total_test_modules);                    \
                UnitTest::total_passed_test_modules = 0;                                                            \
                UnitTest::total_test_modules = 0;                                                                   \
                UnitTest::total_test_asserts = 0;                                                                   \
                UnitTest::total_passed_test_asserts = 0

        class BasicUtilsTests : public UnitTest
        {
        public:
            void RunTest()
            {
                SDL_Log("wrap() tests.");

                TEST_ASSERT(wrap(2, 1, 0, 4) == 3);
                TEST_ASSERT(wrap(2, 2, 0, 4) == 4);
                TEST_ASSERT(wrap(2, 3, 0, 4) == 0);
                TEST_ASSERT(wrap(2, -2, 0, 4) == 0);
                TEST_ASSERT(wrap(2, -3, 0, 4) == 4);
                TEST_ASSERT(wrap(2, 7, 0, 4) == 4);
                TEST_ASSERT(wrap(2, 8, 0, 4) == 0);
                TEST_ASSERT(wrap(2, 9, 0, 4) == 1);
                TEST_ASSERT(wrap(2, 14, 0, 4) == 1);
                TEST_ASSERT(wrap(2, 16, 0, 4) == 3);
                TEST_ASSERT(wrap(2, -5, 0, 4) == 2);
                TEST_ASSERT(wrap(2, -8, 0, 4) == 4);
                TEST_ASSERT(wrap(2, -3, -3, 3) == -1);
                TEST_ASSERT(wrap(2, -6, -3, 3) == 3);
                TEST_ASSERT(wrap(2, -15, -3, 3) == 1);

                SDL_Log("int clamp() tests.");

                TEST_ASSERT(clamp(3, 0, 10) == 3);
                TEST_ASSERT(clamp(-1, 0, 10) == 0);
                TEST_ASSERT(clamp(11, 0, 10) == 10);
            }

        };

        class CircularBufferTests : public UnitTest
        {
        public:
            void RunTest()
            {
                buffer_ints = new CircularBuffer<int>(50);
                for (int i = 0; i < 45; i++)
                {
                    buffer_ints->push_back(i * 3);
                }
                TEST_ASSERT(buffer_ints->size() == 45);
                TEST_ASSERT(buffer_ints->pop_back() == 44 * 3);
                TEST_ASSERT(buffer_ints->size() == 44);

                delete buffer_ints;
                buffer_ints = nullptr;

                buffer_strings = new CircularBuffer<string>(3);

                buffer_strings->push_front("test1");
                buffer_strings->push_front("test2");
                buffer_strings->push_back("test3");

                TEST_ASSERT(buffer_strings->peek_front() == "test2");
                TEST_ASSERT(buffer_strings->peek_back() == "test3");

                buffer_strings->push_front("testoverwrite");

                TEST_ASSERT(buffer_strings->peek_back() == "test1");
                TEST_ASSERT(buffer_strings->peek_front() == "testoverwrite");

                delete buffer_strings;
                buffer_strings = nullptr;

            }

        private:
            CircularBuffer<int>* buffer_ints;
            CircularBuffer<string>* buffer_strings;

        };

    }

}

#endif // TESTMODULES_H
