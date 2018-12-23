#ifndef TESTMODULES_H
#define TESTMODULES_H

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

            /// Total test assertions run in this class
            int total_test_asserts;

            /// Total number of test assertions passed for this class
            int total_passed_test_asserts;

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
                MODULE test_obj_MODULE;                                                                             \
                SDL_Log("\n\nRunning unit test '" #MODULE "':");                                                        \
                test_obj_MODULE.RunTest();                                                                          \
                if (!test_obj_MODULE.passed_test)                                                                   \
                {                                                                                                   \
                    UnitTest::total_passed_test_modules--;                                                          \
                }                                                                                                   \
                SDL_Log("Test module '" #MODULE "' %s!\n\n", test_obj_MODULE.passed_test ? "PASSED" : "FAILED")

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
                SDL_Log("[%s] Test condition '" #TEST_CONDITION "'.", assert_result ? "PASSED" : "!FAILED!" )

        /// When you have finished running all unit tests, use this evaluation macro to log the final results,
        /// pause program execution if any tests failed, and reset the overall unit test results
        #define TEST_EVALUATE()                                                                     \
                SDL_Log("Test evaluation results: Passed %d of %d unit tests.\n",                   \
                        UnitTest::total_passed_test_modules, UnitTest::total_test_modules);         \
                SDL_assert(UnitTest::total_passed_test_modules == UnitTest::total_test_modules);    \
                UnitTest::total_passed_test_modules = 0;                                            \
                UnitTest::total_test_modules = 0

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
            }

        private:
            CircularBuffer<int>* buffer_ints;

        };

    }

}

#endif // TESTMODULES_H
