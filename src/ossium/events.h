#ifndef EVENTS_H
#define EVENTS_H

#include <queue>
#include <string>
#include <unordered_map>
#include <variant>

using namespace std;

namespace ossium
{

    /// Generic event wrapper
    class Event
    {
        /// Constructor takes the event category and sets the unique id
        Event(string setCategory = "");

        /// Adds a key field to the event
        /// e.g. AddKeyField("Health", 100);
        void AddKeyField(string key, variant<int, float, bool, string> value = 0);

        /// Returns the value associated with a given key; defaults to 0 if value not found
        /// and logs an assertion error
        variant<int, float, bool, string> GetValue(string key);

        /// Returns a reference to this event's category type
        const string& getCategory();

        /// Returns a reference to this event's id
        const int& getId();

    private:
        /// Type of event (e.g. 'explosion', 'enemy died')
        string category;

        /// Arguments as a hashtable of key-value pairs
        unordered_map<string, variant<int, float, bool, string>> data;

        /// Unique identifier
        int id;
        static int nextId;
    };

    class EventController
    {
    public:
        EventController();
        ~EventController();

        /// Adds an event to the queue
        void AddEvent(string name);

    private:
        /// Queue of events waiting to be dispatched to listeners
        static queue<Event> events;

    };

}

#endif // EVENTS_H
