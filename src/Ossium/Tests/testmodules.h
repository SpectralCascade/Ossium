#ifndef TESTMODULES_H
#define TESTMODULES_H

#include <string>
#include <unordered_map>
#include <iostream>

#include "../Core/circularbuffer.h"
#include "../Core/tree.h"
#include "../Core/csvdata.h"
#include "../Core/jsondata.h"
#include "../Core/funcutils.h"
#include "../Core/time.h"
#include "../Core/delta.h"
#include "../Core/schemamodel.h"
#include "../Core/randutils.h"
#include "../Core/ecs.h"
#include "../Components/text.h"

using namespace std;

namespace Ossium
{
#ifdef OSSIUM_DEBUG

    namespace Test
    {

        class OSSIUM_EDL UnitTest
        {
        public:
            UnitTest();

            virtual void RunTest() = 0;

            /// Has this module passed the tests or not?
            bool passed_test = true;

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
        #define TEST_RUN(MODULE)                                                                                            \
                Logger::EngineLog().Info("\n\nRunning unit test '" #MODULE "':");                                                            \
                MODULE MODULE##_test_obj;                                                                                   \
                try                                                                                                         \
                {                                                                                                           \
                    MODULE##_test_obj.RunTest();                                                                            \
                }                                                                                                           \
                catch (exception& e)                                                                                        \
                {                                                                                                           \
                    Logger::EngineLog().Error("\n!!!\nException occurred during test module {0}:\n\n{1}\n\n!!!",            \
                                 #MODULE, e.what());                                                                        \
                    MODULE##_test_obj.passed_test = false;                                                                  \
                }                                                                                                           \
                if (!MODULE##_test_obj.passed_test)                                                                         \
                {                                                                                                           \
                    UnitTest::total_passed_test_modules--;                                                                  \
                }                                                                                                           \
                Logger::EngineLog().Info("Test module '" #MODULE "' {0}!\n\n", MODULE##_test_obj.passed_test ? "PASSED" : "FAILED")

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
                Logger::EngineLog().Info("[{0}] Test condition '" #TEST_CONDITION "'.", assert_result ? "PASSED" : "!FAILED!" );

        /// When you have finished running all unit tests, use this evaluation macro to log the final results,
        /// pause program execution if any tests failed, and reset the overall unit test results
        #define TEST_EVALUATE()                                                                                         \
                [] {                                                                                                    \
                    Logger::EngineLog().Info("Test evaluation results: {0} of {1} modules passed tests successfully.",  \
                            UnitTest::total_passed_test_modules, UnitTest::total_test_modules);                         \
                    Logger::EngineLog().Info("{0} of {1} test assertions passed successfully.", UnitTest::total_passed_test_asserts,       \
                            UnitTest::total_test_asserts);                                                              \
                    UnitTest::total_passed_test_modules = 0;                                                            \
                    UnitTest::total_test_modules = 0;                                                                   \
                    UnitTest::total_test_asserts = 0;                                                                   \
                    UnitTest::total_passed_test_asserts = 0;                                                            \
                    return UnitTest::total_passed_test_modules == UnitTest::total_test_modules;                         \
                }()

        class OSSIUM_EDL BasicUtilsTests : public UnitTest
        {
        public:
            void RunTest()
            {
                Logger::EngineLog().Info("Wrap() tests.");

                TEST_ASSERT(Wrap(2, 1, 0, 4) == 3);
                TEST_ASSERT(Wrap(2, 2, 0, 4) == 4);
                TEST_ASSERT(Wrap(2, 3, 0, 4) == 0);
                TEST_ASSERT(Wrap(2, -2, 0, 4) == 0);
                TEST_ASSERT(Wrap(2, -3, 0, 4) == 4);
                TEST_ASSERT(Wrap(2, 7, 0, 4) == 4);
                TEST_ASSERT(Wrap(2, 8, 0, 4) == 0);
                TEST_ASSERT(Wrap(2, 9, 0, 4) == 1);
                TEST_ASSERT(Wrap(2, 14, 0, 4) == 1);
                TEST_ASSERT(Wrap(2, 16, 0, 4) == 3);
                TEST_ASSERT(Wrap(2, -5, 0, 4) == 2);
                TEST_ASSERT(Wrap(2, -8, 0, 4) == 4);
                TEST_ASSERT(Wrap(2, -3, -3, 3) == -1);
                TEST_ASSERT(Wrap(2, -6, -3, 3) == 3);
                TEST_ASSERT(Wrap(2, -15, -3, 3) == 1);

                Logger::EngineLog().Info("int Clamp() tests.");

                TEST_ASSERT(Clamp(3, 0, 10) == 3);
                TEST_ASSERT(Clamp(-1, 0, 10) == 0);
                TEST_ASSERT(Clamp(11, 0, 10) == 10);

                Logger::EngineLog().Info("String functions.");

                TEST_ASSERT(Strip(" testing ") == "testing");
                TEST_ASSERT(Strip(" t        ") == "t");
                TEST_ASSERT(Strip("  test again") == "test again");
                TEST_ASSERT(Strip("test    ") == "test");
                TEST_ASSERT(SplitRight("1:2", ':') == "2");
                TEST_ASSERT(SplitLeft("1:2", ':') == "1");
                TEST_ASSERT(SplitLeft("1111:2", ':') == "1111");
                TEST_ASSERT(SplitLeft("-3:55", ':') == "-3");

                Logger::EngineLog().Info("Type conversion tests.");

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

        class OSSIUM_EDL CircularBufferTests : public UnitTest
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

        class OSSIUM_EDL TreeTests : public UnitTest
        {
        public:
            void RunTest()
            {
                int_tree = new Tree<int>();
                for (int i = 0; i < 50; i++)
                {
                    int_tree->insert("TestNode_" + ToString(i), i * 2);
                }
                vector<Node<int>*> flat_tree = int_tree->getFlatTree();
                for (int i = 0; i < 5; i++)
                {
                    int_tree->insert("TestNode_" + ToString(i + 49), i * 100, flat_tree[49 % (i + 2)]);
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

        class OSSIUM_EDL FSM_TestMachine;

        class FSM_TestState;
/*
        class OSSIUM_EDL FSM_TestMachine : public StateMachine<FSM_TestMachine>, public EventHandler
        {
        public:
            void HandleEvent(Event event)
            {
                if (event.GetCategory() == "state change")
                {
                    if (event.Contains("Health"))
                    {
                        Logger::EngineLog().Info("State change event! Health is set to {0}", *(event.Get<int>("Health")));
                        AddState<FSM_TestState>("meh");
                        SwitchState("meh");
                    }
                }
                else
                {
                    if (event.Contains("Name"))
                    {
                        string f = *(event.Get<string>("Name"));
                        Logger::EngineLog().Info("Hello from event '{0}'!", f);
                    }
                }
                Logger::EngineLog().Info("HANDLED EVENT in category '{0}'.", event.GetCategory());
            }
        };

        class OSSIUM_EDL FSM_TestState : public StateInterface<FSM_TestMachine>
        {
        public:
            FSM_TestState(FSM_TestMachine* machine) : StateInterface(machine)
            {
            }

            void EnterState()
            {
                Logger::EngineLog().Info("EnterState() called on state '{0}'.", name);
            }

            void ExecuteState()
            {
                Logger::EngineLog().Info("ExecuteState() called on state '{0}'.", name);
            }

            void ExitState()
            {
                Logger::EngineLog().Info("ExitState() called on state '{0}'.", name);
            }

        };

        class OSSIUM_EDL FSM_Tests : public UnitTest
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

        class OSSIUM_EDL EventSystemTests : public UnitTest
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

        class OSSIUM_EDL CSV_Tests : public UnitTest
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
*/
        class OSSIUM_EDL JSON_Tests : public UnitTest
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

        class OSSIUM_EDL ClockTests : public UnitTest
        {
        public:
            void RunTest()
            {
                Clock c;
                c.SetWrap(400);
                c.Update(0.049f);
                Logger::EngineLog().Info("{0}", c.GetTime());
                TEST_ASSERT(c.GetTime() == 49);
                c.Update(0.346f);
                TEST_ASSERT(c.GetTime() == 395);
                c.Update(0.005f);
                TEST_ASSERT(c.GetTime() == 400);
                c.Update(0.002f);
                TEST_ASSERT(c.GetTime() == 1);
                c.Scale(-1.0f);
                c.Update(0.002f);
                TEST_ASSERT(c.GetTime() == 400);
                c.Scale(-2.0f);
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

        class OSSIUM_EDL Example : public SchemaRoot, public SchemaExample
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

            M(vector<int>, testVector2) = {15, 20, 25, 30, 0, -1};

        };

        class OSSIUM_EDL InheritanceExample : public Example, public OtherOtherSchema
        {
        public:
            CONSTRUCT_SCHEMA(Example, OtherOtherSchema);

            void SomeOtherMethod()
            {
                Logger::EngineLog().Info("Iterating over {0} fresh derived members:", GetMemberCount());
                for (unsigned int i = 0; i < GetMemberCount(); i++)
                {
                    Logger::EngineLog().Info("Found member '{0}' of type '{1}'", GetMemberName(i), GetMemberType(i));
                    if (GetMemberType(i) == SID("float")::str)
                    {
                        Logger::EngineLog().Info("Member value is: {0}", *((float*)GetMember(i)));
                    }
                    else if (GetMemberType(i) == SID("int")::str)
                    {
                        Logger::EngineLog().Info("Member value is: {0}", *((int*)GetMember(i)));
                    }
                    else if (GetMemberType(i) == SID("string")::str)
                    {
                        Logger::EngineLog().Info("Member value is: {0}" + *((string*)GetMember(i)));
                    }
                }
            }

        };

        class OSSIUM_EDL SchemaTests : public UnitTest, public InheritanceExample
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
                SerialiseIn(data);
                cout << "After serialising, foo == " << foo << " and degree == " << degree << endl << endl;

                cout << "Serialising OUT file 'assets/test_serialise_out.json'..." << endl << endl;
                JSON output;
                SerialiseOut(output);
                output.Export("assets/test_serialise_out.json");
            }

        };

        class OSSIUM_EDL RandTests : public UnitTest
        {
        public:
            void RunTest()
            {
                Rand rng(12345);
                TEST_ASSERT(rng.Int(-55, 142) == 129);
                TEST_ASSERT(rng.Int(2, 4) == 4)
                TEST_ASSERT(ToString(rng.Vector2()) == "(0.316376, 0.130707)");
                TEST_ASSERT(ToString(rng.UnitVector2()) == "(0.915036, 0.403373)");
                TEST_ASSERT(rng.UnitVector2().Length() == 1.0f);
                cout << "Testing RNG values after seeding again..." << endl;
                rng.Seed(12345);
                TEST_ASSERT(rng.Int(-55, 142) == 129);
                TEST_ASSERT(rng.Int(2, 4) == 4);
                TEST_ASSERT(ToString(rng.Vector2()) == "(0.316376, 0.130707)");
                TEST_ASSERT(ToString(rng.UnitVector2()) == "(0.915036, 0.403373)");
            }
        };

        class OSSIUM_EDL EntitySerialisationTests : public UnitTest
        {
        public:
            void RunTest()
            {
                EntityComponentSystem ecs;

                JSON data;
                if (data.Import("assets/test_ecs_serialise_in.json"))
                {
                    string raw = data.ToString();

                    cout << "ECS FromString() executing now..." << endl;

                    ecs.FromString(raw);

                    /*Entity* test = ecs.CreateEntity();
                    test->AddComponent<Text>()->testEnt = ecs.CreateEntity();
                    test->GetComponent<Text>()->testEnt->AddComponent<Texture>();
                    test->GetComponent<Text>()->testComp = test->GetComponent<Text>()->testEnt->GetComponent<Texture>();*/

                    cout << "ECS ToString(): " << ecs.ToString() << endl;

                    JSON output(ecs.ToString());
                    output.Export("assets/test_ecs_serialise_out.json");
                }

            }
        };

    }
#endif
}

#endif // TESTMODULES_H
