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
        /// Constructor takes the event category and the id of the entity that triggered the event
        Event(int originId, string setCategory = "", float delay = 0);

        /// Sets the value of data associated with a specified key
        /// e.g. AddKeyField("ChangePlayerHealth", -30);
        void AddKeyField(string key, variant<int, float, bool, string> value = "");

        /// Returns the value associated with a given key; defaults to 0 if value not found
        /// and logs an assertion error
        variant<int, float, bool, string> GetValue(string key);

        /// Returns a reference to this event's category type
        const string& getCategory();

        /// Returns a reference to this event's id
        const int& getId();

        /// Returns the id of the entity that triggered this event
        const int& getOriginId();

    private:
        /// No need for copy constructors as we are not dealing with pointers,
        /// thus it's safe to copy events using the default copy constructor and assignment operator

        /// Type of event (e.g. 'explosion', 'enemy died')
        string category;

        /// Hash table of key-value data
        unordered_map<string, variant<int, float, bool, string>> data;

        /// Unique id of the entity that triggered this event
        int origin;

        /// How long until this event should be broadcast or dispatched, measured in seconds
        float delayTime;

        /// Unique identifier for this event
        int id;
        static int nextId;
    };

    class EventController
    {
    public:
        EventController();
        ~EventController();

        /// Broadcasts an event if delay time = 0, otherwise adds to the queue for broadcasting later
        void Broadcast(string category);

        /// Dispatches an event directly to a specified entity; immediately if delay = 0, otherwise adds to queue
        void Dispatch(string category, int target_id);

        /// Updates events that are delayed and dispatches them if their delay time is complete
        void DispatchDelayed();

        /// Sub/unsub interested entities to a particular category of events.
        /// If category is an empty string, assumes the entity listens to ALL events!
        void Subscribe(Entity* listener, string category);
        void UnSubscribe(Entity* listener, string category);

    private:
        /// Queue of events waiting to be dispatched to specific listeners, sorted smallest-biggest by delay time
        static set<Event> dispatch_queue;



    };

}

#endif // EVENTS_H
