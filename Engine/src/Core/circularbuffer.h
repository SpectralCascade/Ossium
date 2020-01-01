/** COPYRIGHT NOTICE
 *
 *  Ossium Engine
 *  Copyright (c) 2018-2020 Tim Lane
 *
 *  This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *
 *  3. This notice may not be removed or altered from any source distribution.
 *
**/
#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <algorithm>

#include "funcutils.h"
#include "logging.h"

namespace Ossium
{

    template<class T>
    class OSSIUM_EDL CircularBuffer
    {
    public:
        CircularBuffer(unsigned int length)
        {
            if (length <= 1)
            {
                Logger::EngineLog().Error("CircularBuffer cannot be initialised with a length of 1 or less!");
                #ifdef OSSIUM_DEBUG
                /// Chucked in the SDL assert as it's useful to have in debug builds
                SDL_assert(length > 1);
                #endif // DEBUG
                length = 2;
            }
            max_size = length;
            buffer = new T[max_size];
            count = 0;
            front = 0;
            back = 0;
        }

        CircularBuffer(const CircularBuffer& src)
        {
            max_size = src.max_size;
            buffer = new T[max_size];
            count = 0;
            front = 0;
            back = 0;
            for (unsigned int i = 0; i < max_size; i++)
            {
                buffer[i] = src.buffer[i];
            }
        }

        void swap(CircularBuffer& a, CircularBuffer& b)
        {
            std::swap(a.max_size, b.max_size);
            std::swap(a.buffer, b.buffer);
            std::swap(a.count, b.count);
            std::swap(a.front, b.front);
            std::swap(a.back, b.back);
        }

        CircularBuffer& operator=(CircularBuffer src)
        {
            swap(*this, src);
            return *this;
        }

        ~CircularBuffer()
        {
            delete[] buffer;
        }

        /// Pushes (adds) data to the back of the buffer
        void push_back(const T& data)
        {
            if (count != 0)
            {
                back = Wrap(back, 1, 0, max_size - 1);
                if (back == front)
                {
                    front = Wrap(front, 1, 0, max_size - 1);
                }
                else
                {
                    count++;
                }
            }
            else
            {
                count++;
            }
            buffer[back] = data;
        }

        /// Pushes (adds) data to the front of the buffer
        void push_front(const T& data)
        {
            if (count != 0)
            {
                front = Wrap(front, -1, 0, max_size - 1);
                if (back == front)
                {
                    back = Wrap(back, -1, 0, max_size - 1);
                }
                else
                {
                    count++;
                }
            }
            else
            {
                count++;
            }
            buffer[front] = data;
        }

        /// Pops (removes) data from the back of the buffer
        T pop_back()
        {
            /// We don't want to wrap around if there are no items left in the buffer
            if (count <= 0)
            {
                Logger::EngineLog().Error("Cannot pop_back from an empty circular buffer!");
                /// Return whatever junk value remains rather than throw an exception
                return buffer[back];
            }
            count--;
            int origin_back = back;
            back = Wrap(back, -1, 0, max_size - 1);
            if (front == back)
            {
                front = Wrap(front, -1, 0, max_size - 1);
            }
            return buffer[origin_back];
        }

        T pop_front()
        {
            /// We don't want to wrap around if there are no items left in the buffer
            if (count <= 0)
            {
                Logger::EngineLog().Error("Cannot pop_front from an empty circular buffer!");
                /// Return whatever junk value rather than throw an exception
                return buffer[front];
            }
            count--;
            int origin_front = front;
            front = Wrap(front, 1, 0, max_size - 1);
            if (back == front)
            {
                back = Wrap(back, 1, 0, max_size - 1);
            }
            return buffer[origin_front];
        }

        /// Drops the back to the index argument (relative to the front), and recalculates the size
        /// Useful for stack-style management. Returns false if index out of range. If index < 0, clears the buffer
        bool drop_back_to(int index)
        {
            if (index < count && index >= 0)
            {
                count = index + 1;
                back = Wrap(front, index, 0, max_size - 1);
                return true;
            }
            Logger::EngineLog().Warning("CircularBuffer drop_back index out of range, cannot drop back.");
            return false;
        }

        /// Returns the front index
        int GetFrontIndex()
        {
            return front;
        }

        /// Returns the back index
        int GetBackIndex()
        {
            return back;
        }

        /// Returns a reference to the front and back of the buffer respectively
        const T& peek_back()
        {
            return buffer[back];
        }
        const T& peek_front()
        {
            return buffer[front];
        }

        /// Returns the length of the buffer
        int maxSize()
        {
            return max_size;
        }

        /// Returns the number of elements contained in the buffer
        int size()
        {
            return count;
        }

        /// Returns a constant reference to whatever data resides at the specified index
        /// This is based on relative distance from front to back (i.e. index 0 == front)
        const T& operator[](int index)
        {
            #ifdef OSSIUM_DEBUG
            SDL_assert(index < max_size);
            SDL_assert(index >= 0);
            #endif // DEBUG
            if (count == 0 || front + index > count)
            {
                Logger::EngineLog().Warning("Attempted to access undefined data. Returning back as default.");
                return buffer[back];
            }
            return buffer[front + Wrap(front, index, 0, max_size)];
        }

    private:
        /// Array of data
        T* buffer;

        /// Front index of the buffer
        int front;
        /// End index of the buffer
        int back;
        /// Length of the buffer
        int max_size;

        /// Total number of elements in the buffer
        int count;

    };

}

#endif // CIRCULARBUFFER_H
