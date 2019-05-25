#include <unordered_map>
#include <string>
#include <SDL.h>

#include "events.h"
#include "basics.h"

using namespace std;

namespace Ossium
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
        for (auto i = event_data.data.begin(); i != event_data.data.end(); i++)
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
                    Add((*i)[0], ToInt((*i)[1]));
                }
                else if (IsFloat((*i)[1]))
                {
                    Add((*i)[0], ToFloat((*i)[1]));
                }
                else
                {
                    Add((*i)[0], (*i)[1]);
                }
            }
        }
    }

    void Event::Add(string key, variant<string, int, float> value)
    {
        data[key] = value;
    }

    variant<string, int, float>* Event::GetValue(string key)
    {
        auto value = data.find(key);
        if (value != data.end())
        {
            return &value->second;
        }
        return nullptr;
    }

    string Event::Get(string key)
    {
        auto value = GetValue(key);
        if (value != nullptr)
        {
            try
            {
                if (holds_alternative<int>(*value))
                {
                    return functions::ToString(get<int>(*value));
                }
                else if (holds_alternative<float>(*value))
                {
                    return functions::ToString(get<float>(*value));
                }
                else
                {
                    return get<string>(*value);
                }
            }
            catch (bad_variant_access&)
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Bad variant access in event! Key used was \"%s\".", key.c_str());
            }
        }
        return "null";
    }

    bool Event::Contains(string key)
    {
        return data.find(key) != data.end();
    }

    const string& Event::GetCategory()
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

    string Event::ToString()
    {
        stringstream jsonStream;
        jsonStream.str("");
        jsonStream << "{";
        if (!data.empty())
        {
            for (auto itr = data.begin(); itr != data.end();)
            {
                jsonStream << endl << "    \"" << itr->first << "\"" << " : ";
                if (holds_alternative<string>(itr->second))
                {
                    string str = get<string>(itr->second);
                    jsonStream << "\"" << str << "\"";
                }
                else if (holds_alternative<int>(itr->second))
                {
                    jsonStream << get<int>(itr->second);
                }
                else
                {
                    jsonStream << get<float>(itr->second);
                }
                if (++itr != data.end())
                {
                    jsonStream << ",";
                }
            }
        }
        jsonStream << endl;
        jsonStream << "}";
        return jsonStream.str();
    }

    void Event::FromString(string& str)
    {
        data.clear();
        JSON json;
        if (json.Parse(str))
        {
            for (auto itr = json.begin(); itr != json.end(); itr++)
            {
                if (itr->second.IsInt())
                {
                    Add(itr->first, itr->second.ToInt());
                }
                else if (itr->second.IsFloat())
                {
                    Add(itr->first, itr->second.ToFloat());
                }
                else
                {
                    Add(itr->first, (string)itr->second);
                }
            }
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Failed to serialise event from a JSON string.");
        }
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
        EventCategoryInfo& info = registry[event.GetCategory()];
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
            event.sendTime = info.category_clock->GetTime() + delay;
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
            EventCategoryInfo& info = registry[event.GetCategory()];
            /// Put the event in the dispatch queue
            if (info.category_clock == nullptr)
            {
                /// Use default clock if none is provided
                info.category_clock = &events_clock;
            }
            EventMessage message;
            message.data = event.data;
            message.category = event.category;
            message.sendTime = info.category_clock->GetTime() + delay;
            message.target = target;
            info.dispatch_queue.insert(message);
        }
    }

    void EventController::Update(float deltaTime)
    {
        events_clock.Update(deltaTime);
        for (auto info = registry.begin(); info != registry.end(); info++)
        {
            for (auto i = (*info).second.dispatch_queue.begin(); i != (*info).second.dispatch_queue.end() && (*i).sendTime < (*info).second.category_clock->GetTime(); i++)
            {
                (*i).target->HandleEvent(*i);
                (*info).second.dispatch_queue.erase(i);
            }
            for (auto i = (*info).second.broadcast_queue.begin(); i != (*info).second.broadcast_queue.end() && (*i).sendTime < (*info).second.category_clock->GetTime(); i++)
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
