#ifndef TESTMODULES_H
#define TESTMODULES_H

#include <string>
#include <unordered_map>
#include <iostream>

#include "circularbuffer.h"
#include "tree.h"
#include "fsm.h"
#include "events.h"
#include "csvdata.h"
#include "jsondata.h"
#include "basics.h"
#include "time.h"
#include "delta.h"
#include "schemamodel.h"
#include "randutils.h"

using namespace std;

namespace Ossium
{
#ifdef UNIT_TESTS

    namespace Test
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

                SDL_Log("String functions.");

                TEST_ASSERT(strip(" testing ") == "testing");
                TEST_ASSERT(strip(" t        ") == "t");
                TEST_ASSERT(strip("  test again") == "test again");
                TEST_ASSERT(strip("test    ") == "test");
                TEST_ASSERT(splitPair("1:2", ':') == "2");
                TEST_ASSERT(splitPairFirst("1:2", ':') == "1");
                TEST_ASSERT(splitPairFirst("1111:2", ':') == "1111");
                TEST_ASSERT(splitPairFirst("-3:55", ':') == "-3");

                SDL_Log("Type conversion tests.");

                TEST_ASSERT(IsInt("1234"));
                TEST_ASSERT(IsFloat("142.04"));
                TEST_ASSERT(!IsInt("234.53"));
                TEST_ASSERT(!IsInt("words"));
                TEST_ASSERT(IsInt("-412"));
                TEST_ASSERT(!IsFloat("more words"));
                TEST_ASSERT(IsFloat("-142.0"));
                TEST_ASSERT(IsFloat("1.7102e20"));
                TEST_ASSERT(!IsFloat("1e"));
                TEST_ASSERT(IsFloat("-1.2523e14"));

                TEST_ASSERT(ToInt("1532") == 1532);
                TEST_ASSERT(ToFloat("15.25") == 15.25f);
                TEST_ASSERT(ToFloat("-21.9232") == -21.9232f);
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

        class TreeTests : public UnitTest
        {
        public:
            void RunTest()
            {
                int_tree = new Tree<int>();
                for (int i = 0; i < 50; i++)
                {
                    int_tree->add("TestNode_" + ToString(i), i * 2);
                }
                vector<Node<int>*> flat_tree = int_tree->getFlatTree();
                for (int i = 0; i < 5; i++)
                {
                    int_tree->add("TestNode_" + ToString(i + 49), i * 100, flat_tree[49 % (i + 2)]);
                    TEST_ASSERT(int_tree->find("TestNode_" + ToString(i + 49))->data == i * 100);
                }
                vector<string> test_strings;

                test_strings.push_back("TestNode_50");
                test_strings.push_back("TestNode_52");
                test_strings.push_back("TestNode_18");

                TEST_ASSERT(!int_tree->findAll(test_strings).empty());
                TEST_ASSERT(int_tree->findAll(test_strings).size() == 3);

                delete int_tree;
                int_tree = nullptr;
            }

        private:
            Tree<int>* int_tree;

        };

        class FSM_TestMachine;

        class FSM_TestState;

        class FSM_TestMachine : public StateMachine<FSM_TestMachine>, public EventHandler
        {
        public:
            void HandleEvent(Event event)
            {
                if (event.GetCategory() == "state change")
                {
                    if (event.Contains("Health"))
                    {
                        SDL_Log("State change event! Health is set to %d", *(event.Get<int>("Health")));
                        AddState<FSM_TestState>("meh");
                        SwitchState("meh");
                    }
                }
                else
                {
                    if (event.Contains("Name"))
                    {
                        string f = *(event.Get<string>("Name"));
                        SDL_Log("Hello from event '%s'!", f.c_str());
                    }
                }
                SDL_Log("HANDLED EVENT in category '%s'.", event.GetCategory().c_str());
            }
        };

        class FSM_TestState : public StateInterface<FSM_TestMachine>
        {
        public:
            FSM_TestState(FSM_TestMachine* machine) : StateInterface(machine)
            {
            }

            void EnterState()
            {
                SDL_Log("EnterState() called on state '%s'.", name.c_str());
            }

            void ExecuteState()
            {
                SDL_Log("ExecuteState() called on state '%s'.", name.c_str());
            }

            void ExitState()
            {
                SDL_Log("ExitState() called on state '%s'.", name.c_str());
            }

        };

        class FSM_Tests : public UnitTest
        {
        public:
            void RunTest()
            {
                test_obj.AddState<FSM_TestState>("State 1");
                test_obj.AddState<FSM_TestState>("Default State");
                test_obj.SwitchState("Default State");
                test_obj.UpdateFSM();
                TEST_ASSERT(test_obj.CurrentState()->name == "Default State");
                test_obj.SwitchState("State 1");
                test_obj.SetPreState<FSM_TestState>();
                test_obj.SetPostState<FSM_TestState>();
                test_obj.UpdateFSM();
                TEST_ASSERT(test_obj.CurrentState()->name == "State 1");
            }

        private:
            FSM_TestMachine test_obj;

        };

        class EventSystemTests : public UnitTest
        {
        public:
            void RunTest()
            {
                test_obj.AddState<FSM_TestState>();
                test_obj.SwitchState();
                test_obj.UpdateFSM();

                Event myevent;
                myevent.Init("Bob events");
                myevent.Add("Name", (string)"bob event");
                myevent.Add("Health", 100);

                test_obj.DispatchEvent(myevent, &test_obj);

                myevent.Init("state change");

                test_obj.SubscribeEvent("state change");
                test_obj.BroadcastEvent(myevent);
                Uint32 lastTime = SDL_GetTicks();
                while (test_obj.CurrentState()->name == "Default State")
                {
                    float delta = (float)(SDL_GetTicks() - lastTime) / 1000.0f;
                    EventHandler::_event_controller.Update(delta);
                    lastTime = SDL_GetTicks();
                }
            }

        private:
            FSM_TestMachine test_obj;

        };

        class CSV_Tests : public UnitTest
        {
        public:
            void RunTest()
            {
                CSV csv;
                csv.Import("test.csv", false);
                TEST_ASSERT(csv.GetCell(0, 0) == "Test 1");
                TEST_ASSERT(csv.GetCell(0, 1) == "Test 2");
                TEST_ASSERT(csv.GetCell(0, 2) == "Test 3");
                TEST_ASSERT(csv.GetCell(1, 1) == "3");
                TEST_ASSERT(csv.GetCell(2, 1) == "41.3");
                TEST_ASSERT(csv.GetCell(2, 2) == "0.4");
                TEST_ASSERT(csv.GetCell(3, 2) == "105");
                TEST_ASSERT(csv.GetCell(5, 2) == "");
                TEST_ASSERT(csv.GetCell(6, 2) == "43");

                csv.data[0][1] = "TESTING OUTPUT";
                csv.Export("test_output.csv");
                csv.Import("test_output.csv");
                TEST_ASSERT(csv.GetCell(0, 1) == "TESTING OUTPUT");

                csv.Import("event_test.csv", true);
                TEST_ASSERT(csv.GetCell(2, 0) == "Name");
                Event myevent;
                myevent.Init(csv);
                TEST_ASSERT(myevent.GetCategory() == "npc_talk_question_rhetorical");
                TEST_ASSERT(*(myevent.Get<string>("Name")) == "Bob");
            }

        };

        class JSON_Tests : public UnitTest
        {
        public:
            void RunTest()
            {
                JSON json;
                TEST_ASSERT(json.Import("assets/test.json"));
                TEST_ASSERT(json["Key"] == "Value");
                TEST_ASSERT(json["Wow"] == "Nice");
                TEST_ASSERT(json["Number"].ToFloat() == 15.0f);
                TEST_ASSERT(json["Array"].IsArray());
                TEST_ASSERT(json["Array"].ToArray()[2] == "{\"fake\" : 0, \"obj\" : false}");
                json["new key"] = ToString(14);
                json.Export("assets/test_out.json");
                json.clear();
                json.Import("assets/test_out.json");
                TEST_ASSERT(json["new key"] == "14");
            }
        };

        class ClockTests : public UnitTest
        {
        public:
            void RunTest()
            {
                Clock c;
                c.SetWrap(400);
                c.Update(0.049f);
                SDL_Log("%d", (int)c.GetTime());
                TEST_ASSERT(c.GetTime() == 49);
                c.Update(0.346f);
                TEST_ASSERT(c.GetTime() == 395);
                c.Update(0.005f);
                TEST_ASSERT(c.GetTime() == 400);
                c.Update(0.002f);
                TEST_ASSERT(c.GetTime() == 1);
                c.Stretch(-1.0f);
                c.Update(0.002f);
                TEST_ASSERT(c.GetTime() == 400);
                c.Stretch(-2.0f);
                c.Update(0.100f);
                TEST_ASSERT(c.GetTime() == 200);
                c.Update(0.200f);
                TEST_ASSERT(c.GetTime() == 201);
                c.SetWrap(399);
                c.Update(0.200f);
                TEST_ASSERT(c.GetTime() == 201);
            }

        };

        ///
        /// Type schema tests/demo
        ///

        struct SchemaExample : public Schema<SchemaExample>
        {
            DECLARE_SCHEMA(SchemaExample, Schema<SchemaExample>);

            M(int, foo) = 16;

            M(int, bar) = 888;

            M(float, age) = 19.02f;

            M(string, hello) = "hello world!";

            M(int, oh) = 999;

            M(float, more) = 555.3f;
        };

        class Example : public SchemaRoot, public SchemaExample
        {
        public:
            CONSTRUCT_SCHEMA(SchemaRoot, SchemaExample);

        };

        struct OtherSchema : public Schema<OtherSchema>
        {
            DECLARE_SCHEMA(OtherSchema, Schema<OtherSchema>);

            M(int, freshFoo) = 1920;

            M(float, wow) = -0.75f;
        };

        struct OtherOtherSchema : public OtherSchema
        {
            DECLARE_SCHEMA(OtherOtherSchema, OtherSchema);

            M(string, degree) = "Computer Science";

            M(string, freshHello) = "HELLO FROM DERIVED SCHEMA :D";

            M(vector<int>, testVector) = {15, 20, 25, 30, 0, -1};

        };

        class InheritanceExample : public Example, public OtherOtherSchema
        {
        public:
            CONSTRUCT_SCHEMA(Example, OtherOtherSchema);

            void SomeOtherMethod()
            {
                SDL_Log("Iterating over %d fresh derived members:", GetMemberCount());
                for (unsigned int i = 0; i < GetMemberCount(); i++)
                {
                    SDL_Log("Found member '%s' of type '%s'", GetMemberName(i), GetMemberType(i));
                    if (GetMemberType(i) == SID("float")::str)
                    {
                        cout << "Member value is: " << *((float*)GetMember(i)) << endl;
                    }
                    else if (GetMemberType(i) == SID("int")::str)
                    {
                        cout << "Member value is: " << *((int*)GetMember(i)) << endl;
                    }
                    else if (GetMemberType(i) == SID("string")::str)
                    {
                        cout << "Member value is: " << *((string*)GetMember(i)) << endl;
                    }
                }
            }

        };

        class SchemaTests : public UnitTest, public InheritanceExample
        {
        public:
            void RunTest()
            {
                TEST_ASSERT(GetMemberCount() == 11);

                /// Output member offsets information
                for (unsigned int i = 0; i < GetMemberCount(); i++)
                {
                    cout << "Offset of member " << GetMemberName(i) << " is " << (size_t)GetMember(i) - (size_t)this << endl;
                }

                //SomeOtherMethod();
                JSON data;
                data.Import("assets/test_serialised_in.json");
                cout << endl << "Before serialising, foo == " << foo << " and degree == " << degree << endl;
                SerialiseSchema(&data);
                cout << "After serialising, foo == " << foo << " and degree == " << degree << endl << endl;

                cout << "Serialising OUT file 'assets/test_serialise_out.json'..." << endl << endl;
                JSON* output = SerialiseSchema();
                output->Export("assets/test_serialise_out.json");
                delete output;
                output = nullptr;

            }

        };

        class RandTests : public UnitTest
        {
        public:
            void RunTest()
            {
                Rand rng(12345);
                TEST_ASSERT(rng.Int(-55, 142) == 129);
                TEST_ASSERT(rng.Int(2, 4) == 4)
                TEST_ASSERT(ToString(rng.Vector()) == "(0.316376, 0.130707)");
                TEST_ASSERT(ToString(rng.UnitVector()) == "(0.915036, 0.403373)");
                TEST_ASSERT(rng.UnitVector().Magnitude() == 1.0f);
                cout << "Testing RNG values after seeding again..." << endl;
                rng.Seed(12345);
                TEST_ASSERT(rng.Int(-55, 142) == 129);
                TEST_ASSERT(rng.Int(2, 4) == 4)
                TEST_ASSERT(ToString(rng.Vector()) == "(0.316376, 0.130707)");
                TEST_ASSERT(ToString(rng.UnitVector()) == "(0.915036, 0.403373)");
            }
        };

    }
#endif
}

#endif // TESTMODULES_H
