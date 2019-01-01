#ifndef EVENTS_H
#define EVENTS_H

#include <set>
#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <variant>

#include "time.h"
#include "csvdata.h"

using namespace std;

namespace ossium
{

    /// Forward declaration
    class EventController;

    /// Generic event object
    class Event
    {
    public:
        friend class EventController;

        /// Initialiser takes the event category
        void Init(string _category);
        /// Overload takes preloaded CSV data
        void Init(CSV& event_data);

        /// Sets the value of data associated with a specified key
        /// e.g. AddKeyField("ChangePlayerHealth", -30);
        void AddKeyField(string key, variant<string, int, float> value = (string)"");

        /// Returns pointer to the value associated with a given key; returns null if no such key exists!
        variant<string, int, float>* GetValue(string key);

        /// Returns a reference to this event's category type
        const string& getCategory();

        friend bool operator<(const Event& first, const Event& second);

        /// For sorting when this event is queued
        bool operator<(const Event& other);

    protected:
        /// No need for copy constructors as we are not dealing with pointers,
        /// thus it's safe to copy events using the default copy constructor and assignment operator
        /// I've tried to keep the class fairly lightweight so that it copies easily

        /// Type of event (e.g. 'explosion', 'enemy died')
        string category;

        /// When this event should be broadcast or dispatched, measured in milliseconds relative to eventClock
        Uint32 sendTime;

        /// Table of key-value arguments
        map<string, variant<string, int, float>> data;

    };

    /// An interface class for handling generic game events
    class EventHandler
    {
    public:
        virtual void HandleEvent(Event event) = 0;

        /// delay == 0 results in immediate event handling, otherwise event is queued
        void BroadcastEvent(Event event, Uint32 delay = 0);

        /// delay == 0 results in immediate event handling, otherwise event is queued
        void DispatchEvent(Event event, EventHandler* target, Uint32 delay = 0);

        /// Register or un-register interest in a specific category of events
        void SubscribeEvent(string category);
        void UnsubscribeEvent(string category);

        /// The event controller itself
        static EventController _event_controller;

    };

    /// An event intended to be sent directly to a specific event handler
    class EventMessage : public Event
    {
        friend class EventController;

        /// Initialise pointer to null
        EventMessage();

        /// Target event handler
        EventHandler* target;
    };

    struct EventCategoryInfo
    {
        /// Queue of events to be dispatched to a specified event handler
        set<EventMessage> dispatch_queue;

        /// Queue of events to be broadcast in this category
        set<Event> broadcast_queue;

        /// The handlers subscribed to this category
        vector<EventHandler*> subscribers;

        /// The clock used to compute event delay times for this category
        Clock* category_clock = nullptr;

    };

    class EventController
    {
    public:
        EventController(){};
        ~EventController();

        /// Broadcasts an event. If delay == 0, immediately broadcasts. Otherwise queues the event.
        void Broadcast(Event event, Uint32 delay = 0);
        /// Dispatches an event directly to a specified event handler; delay applies in the same way as Broadcast()
        void Dispatch(Event event, EventHandler* target, Uint32 delay = 0);

        /// Updates events that are delayed and broadcasts/dispatches them if their delay time is complete
        /// Also updates the default delay clock
        void Update(float deltaTime);

        /// Sub/unsub interested EventHandlers to a particular category of events.
        void Subscribe(EventHandler* listener, const string& category);
        void Unsubscribe(EventHandler* listener, const string& category);

        /// Sets the clock for a specified category
        void SetCategoryClock(string category, Clock* clock);

    private:
        EventController(const EventController& src);
        EventController& operator=(const EventController& src);

        /// Registry of queued events and event subscribers, by category
        unordered_map<string, EventCategoryInfo> registry;

        /// Default real time clock used when calculating delay
        Clock events_clock;

    };

}

#endif // EVENTS_H
