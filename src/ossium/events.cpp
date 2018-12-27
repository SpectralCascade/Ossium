#include <unordered_map>
#include <variant>
#include <string>
#include <SDL2/SDL.h>

#include "events.h"

using namespace std;

namespace ossium
{

    int Event::nextId = 0;

    Event::Event(string setCategory)
    {
        category = setCategory;
        id = nextId;
        nextId++;
    }

    void Event::AddKeyField(string key, variant<int, float, bool, string> value)
    {
        data[key] = value;
    }

    variant<int, float, bool, string> Event::GetValue(string key)
    {
        if (data.find(key) != data.end())
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

}
