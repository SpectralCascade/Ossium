#include <unordered_map>
#include <string>
#include <SDL2/SDL.h>
#include <variant>

#include "events.h"
#include "basics.h"

using namespace std;

namespace ossium
{

    ///
    /// Event
    ///

    void Event::Init(string _category)
    {
        category = _category;
    }

    void Event::Init(CSV& event_data)
    {
        for (auto i = event_data.Data().begin(); i != event_data.Data().end(); i++)
        {
            if ((*i).empty() || (*i).size() < 2)
            {
                SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "Failed to parse event key-value field.");
                continue;
            }
            if ((*i)[0][0] == '%')
            {
                if ((*i)[0] == "%category%")
                {
                    category = (*i)[1];
                }
            }
            else
            {
                /// Add the key-value pair to the event
                if (IsInt((*i)[1]))
                {
                    AddKeyField((*i)[0], ToInt((*i)[1]));
                }
                else if (IsFloat((*i)[1]))
                {
                    AddKeyField((*i)[0], ToFloat((*i)[1]));
                }
                else
                {
                    AddKeyField((*i)[0], (*i)[1]);
                }
            }
        }
    }

    void Event::AddKeyField(string key, variant<string, int, float> value)
    {
        data[key] = value;
    }

    variant<string, int, float>* Event::GetValue(string key)
    {
        auto value = data.find(key);
        if (value != data.end())
        {
            return &data[key];
        }
        return nullptr;
    }

    const string& Event::getCategory()
    {
        return category;
    }

    bool operator<(const Event& first, const Event& second)
    {
        return first.sendTime < second.sendTime;
    }

    bool Event::operator<(const Event& other)
    {
        return sendTime < other.sendTime;
    }

    ///
    /// EventMessage
    ///

    EventMessage::EventMessage()
    {
        target = nullptr;
    }

    ///
    /// EventHandler
    ///

    void EventHandler::BroadcastEvent(Event event, Uint32 delay)
    {
        _event_controller.Broadcast(event, delay);
    }

    void EventHandler::DispatchEvent(Event event, EventHandler* target, Uint32 delay)
    {
        _event_controller.Dispatch(event, target, delay);
    }

    void EventHandler::SubscribeEvent(string category)
    {
        _event_controller.Subscribe(this, category);
    }

    void EventHandler::UnsubscribeEvent(string category)
    {
        _event_controller.Unsubscribe(this, category);
    }

    EventController EventHandler::_event_controller;

    ///
    /// EventController
    ///

    EventController::~EventController()
    {
        registry.clear();
    }

    void EventController::Broadcast(Event event, Uint32 delay)
    {
        EventCategoryInfo& info = registry[event.getCategory()];
        if (delay == 0)
        {
            /// Broadcast event immediately to all subscribers
            for (auto i = info.subscribers.begin(); i != info.subscribers.end(); i++)
            {
                (*i)->HandleEvent(event);
            }
        }
        else
        {
            /// Put the event in the broadcast queue
            if (info.category_clock == nullptr)
            {
                /// Use the default clock if none is provided
                info.category_clock = &events_clock;
            }
            event.sendTime = info.category_clock->getTime() + delay;
            info.broadcast_queue.insert(event);
        }
    }

    void EventController::Dispatch(Event event, EventHandler* target, Uint32 delay)
    {
        if (delay == 0)
        {
            /// Dispatch event immediately
            target->HandleEvent(event);
        }
        else
        {
            EventCategoryInfo& info = registry[event.getCategory()];
            /// Put the event in the dispatch queue
            if (info.category_clock == nullptr)
            {
                /// Use default clock if none is provided
                info.category_clock = &events_clock;
            }
            EventMessage message;
            message.data = event.data;
            message.category = event.category;
            message.sendTime = info.category_clock->getTime() + delay;
            message.target = target;
            info.dispatch_queue.insert(message);
        }
    }

    void EventController::Update(float deltaTime)
    {
        events_clock.update(deltaTime);
        for (auto info = registry.begin(); info != registry.end(); info++)
        {
            for (auto i = (*info).second.dispatch_queue.begin(); i != (*info).second.dispatch_queue.end() && (*i).sendTime < (*info).second.category_clock->getTime(); i++)
            {
                (*i).target->HandleEvent(*i);
                (*info).second.dispatch_queue.erase(i);
            }
            for (auto i = (*info).second.broadcast_queue.begin(); i != (*info).second.broadcast_queue.end() && (*i).sendTime < (*info).second.category_clock->getTime(); i++)
            {
                for (auto j = (*info).second.subscribers.begin(); j != (*info).second.subscribers.end(); j++)
                {
                    (*j)->HandleEvent(*i);
                }
                (*info).second.broadcast_queue.erase(i);
            }
        }
    }

    void EventController::Subscribe(EventHandler* listener, const string& category)
    {
        registry[category].subscribers.push_back(listener);
    }

    void EventController::Unsubscribe(EventHandler* listener, const string& category)
    {
        EventCategoryInfo& info = registry[category];
        for (auto i = info.subscribers.begin(); i != info.subscribers.end(); i++)
        {
            if (*i == listener)
            {
                info.subscribers.erase(i);
                return;
            }
        }
    }

    void EventController::SetCategoryClock(string category, Clock* category_clock)
    {
        registry[category].category_clock = category_clock;
    }

}
