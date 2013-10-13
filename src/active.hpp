/*
 *  This file is part of mal-gtk.
 *
 *  mal-gtk is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  mal-gtk is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with mal-gtk.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include <memory>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <queue>

namespace MAL {

    template <typename T> class message_queue {
    public:
        message_queue<T>() = default;
        void push(const T& t) {
            std::lock_guard<std::mutex> lock(m);
            queue.push(t);
            cond.notify_one();
        };

        void push(T&& t) {
            std::lock_guard<std::mutex> lock(m);
            queue.push(std::forward(t));
            cond.notify_one();
        };

        T pop() {
            std::unique_lock<std::mutex> lock(m);
            while (queue.empty()) {
                cond.wait(lock);
            }
            auto front = queue.front();
            queue.pop();
            return front;
        };

    private:
        std::queue<T> queue;
        std::mutex m;
        std::condition_variable cond;
    };

    class Active {
    public:
        typedef std::function<void()> Message;
        Active( const Active& ) = delete;
        void operator=( const Active& ) = delete;
 
    private:
        bool done;
        message_queue<Message> mq;
        std::unique_ptr<std::thread> thd;
 
        void Run() {
            while( !done ) {
                Message msg = mq.pop();
                msg();            // execute message
            } // note: last message sets done to true
        }
 
    public:
 
        Active() : done(false) {
            thd = std::unique_ptr<std::thread>(
                new std::thread( [=]{ this->Run(); } ) );
        }
 
        ~Active() {
            send( [&]{ done = true; } ); ;
            thd->join();
        }
 
        void send( Message m ) {
            mq.push( m );
        }
    };

}
