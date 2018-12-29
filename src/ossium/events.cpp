#include <unordered_map>
#include <variant>
#include <string>
#include <SDL2/SDL.h>

#include "events.h"

using namespace std;

namespace ossium
{

    int Event::nextId = 0;

    Event::Event(EventHandler* _origin, string _category, Clock* delay_clock)
    {
        eventClock = delay_clock;
        origin = _origin;
        category = _category;
        id = nextId;
        nextId++;
    }

    void Event::AddKeyField(string key, variant<int, float, bool, string> value)
    {
        data[key] = value;
    }

    variant<int, float, bool, string> Event::GetValue(string key)
    {
        auto value = data.find(key);
        if (value != data.end())
        {
            return data[key];
        }
        SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "Data key for Event[id: %d, cat: %s] does not exist!", id, category.c_str());
        return 0;
    }

    const string& Event::getCategory()
    {
        return category;
    }

    const int& Event::getId()
    {
        return id;
    }

    const EventHandler* Event::getOrigin()
    {
        return origin;
    }

    EventMessage::EventMessage(EventHandler* _target, Event& _event)
    {
        target = _target;
        event = _event;
    }

    void EventHandler::BroadcastEvent(Event& event, float delay)
    {
        controller.Broadcast(event, delay);
    }

    void EventHandler::DispatchEvent(Event& event, EventHandler* target, float delay)
    {
        controller.Dispatch(event, target, delay);
    }

    void EventHandler::SubscribeEvent(string category)
    {
        controller.Subscribe(this, category);
    }

    void EventHandler::UnsubscribeEvent(string category)
    {
        controller.Unsubscribe(this, category);
    }

    EventController EventHandler::controller;

    EventController::~EventController()
    {
        event_registry.clear();
        dispatch_queue.clear();
        broadcast_queue.clear();
    }

    void EventController::Broadcast(Event& event, float delay)
    {
        if (delay <= 0)
        {
            /// Broadcast event immediately
            auto i = event_registry.find(event.getCategory());
            if (i != event_registry.end())
            {
                for (auto itr = (*i).second.begin(); itr != (*i).second.end(); itr++)
                {
                    (*itr)->HandleEvent(event);
                }
            }
        }
        else
        {
            /// Put the event in the broadcast queue
            if (event.eventClock == nullptr)
            {
                /// Use default clock if none is provided
                event.eventClock = &events_clock;
            }
            event.sendTime = ((float)event.eventClock->getTime() / 1000.0f) + delay;
            broadcast_queue.push_back(event);
        }
    }

    void EventController::Dispatch(Event& event, EventHandler* target, float delay)
    {
        if (delay <= 0)
        {
            /// Dispatch event immediately
            target->HandleEvent(event);
        }
        else
        {
            /// Put the event in the dispatch queue
            if (event.eventClock == nullptr)
            {
                /// Use default clock if none is provided
                event.eventClock = &events_clock;
            }
            event.sendTime = ((float)event.eventClock->getTime() / 1000.0f) + delay;
            EventMessage message(target, event);
            dispatch_queue.push_back(message);
        }
    }

    void EventController::Update(float deltaTime)
    {
        events_clock.update(deltaTime);
        Uint32 currentTime = events_clock.getTime();
        while (!dispatch_queue.empty() && (*(dispatch_queue.begin())).event.sendTime <= currentTime)
        {
            (*(dispatch_queue.begin())).target->HandleEvent((*(dispatch_queue.begin())).event);
            dispatch_queue.erase(dispatch_queue.begin());
        }
        while (!broadcast_queue.empty() && (*(broadcast_queue.begin())).sendTime <= currentTime)
        {
            if (event_registry.find((*(broadcast_queue.begin())).getCategory()) != event_registry.end())
            {
                auto itr = event_registry.find((*(broadcast_queue.begin())).getCategory());
                if (itr != event_registry.end())
                {
                    vector<EventHandler*> handlers = (*itr).second;
                    for (auto i = handlers.begin(); i != handlers.end(); i++)
                    {
                        (*i)->HandleEvent((*(broadcast_queue.begin())));
                    }
                }
                broadcast_queue.erase(broadcast_queue.begin());
            }
        }
    }

    void EventController::Subscribe(EventHandler* listener, const string& category)
    {
        event_registry[category].push_back(listener);
    }

    void EventController::Unsubscribe(EventHandler* listener, const string& category)
    {
        event_registry.erase(category);
    }

}
