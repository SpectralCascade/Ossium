#ifndef EVENTS_H
#define EVENTS_H

#include <set>
#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <variant>

#include "time.h"

using namespace std;

namespace ossium
{

    /// Forward declarations
    class EventHandler;
    class EventController;

    /// Generic event wrapper
    class Event
    {
    public:
        friend class EventController;

        Event(){};

        /// Constructor takes the event category and the id of the entity that triggered the event
        /// You can pass in a custom clock for delaying events in different time lines e.g. game time instead of real time
        /// If null, any delay applied defaults to real time
        Event(EventHandler* _origin, string _category, Clock* delay_clock = nullptr);

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

        /// Returns pointer to the object that triggered this event
        const EventHandler* getOrigin();

    private:
        /// No need for copy constructors as we are not dealing with pointers,
        /// thus it's safe to copy events using the default copy constructor and assignment operator
        /// I've tried to keep the class fairly lightweight so that it copies easily

        /// Type of event (e.g. 'explosion', 'enemy died')
        string category;

        /// When this event should be broadcast or dispatched, measured in milliseconds relative to eventClock
        Uint32 sendTime;

        /// Hash table of key-value data
        map<string, variant<int, float, bool, string>> data;

        /// Pointer to the entity that triggered this event
        EventHandler* origin;

        /// A clock used specifically for this event
        /// This enables events to be used in different time lines e.g. game time (so events don't fire when paused)
        /// but also real time (so UI events can be delayed in real time)
        Clock* eventClock;

        /// Unique identifier for this event
        int id;
        static int nextId;
    };

    /// An interface class for handling generic game events
    class EventHandler
    {
    public:
        virtual void HandleEvent(Event& event) = 0;

        void BroadcastEvent(Event& event, float delay = 0);

        void DispatchEvent(Event& event, EventHandler* target, float delay = 0);

        void SubscribeEvent(string category);
        void UnsubscribeEvent(string category);

    private:
        static EventController controller;

    };

    /// Wrapper for targeted events
    struct EventMessage
    {
        EventMessage(EventHandler* _target, Event& _event);
        EventMessage(){};
        EventHandler* target;
        Event event;
    };

    class EventController
    {
    public:
        EventController(){};
        ~EventController();

        /// Broadcasts an event if delay time = 0, otherwise adds to the queue for broadcasting later
        void Broadcast(Event& event, float delay = 0);

        /// Dispatches an event directly to a specified event handler; immediately if delay = 0, otherwise adds to queue
        void Dispatch(Event& event, EventHandler* target, float delay = 0);

        /// Updates events that are delayed and dispatches them if their delay time is complete
        /// Also updates the default delay clock
        void Update(float deltaTime);

        /// Sub/unsub interested entities (specifically, a component inheriting from EventHandler) to a particular category of events.
        void Subscribe(EventHandler* listener, const string& category);
        void Unsubscribe(EventHandler* listener, const string& category);

    private:
        EventController(const EventController& src);
        EventController& operator=(const EventController& src);

        /// Set of events waiting to be dispatched to specific listeners
        vector<EventMessage> dispatch_queue;
        /// Set of events waiting to be broadcast by category
        vector<Event> broadcast_queue;

        /// Registry of subscribed event handlers by event category
        unordered_map<string, vector<EventHandler*>> event_registry;

        /// Default real time clock used when calculating delay
        Clock events_clock;

    };

}

#endif // EVENTS_H
