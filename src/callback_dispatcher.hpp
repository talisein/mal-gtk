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
#include <mutex>
#include <queue>
#include <functional>
#include <glibmm/dispatcher.h>

namespace MAL {

    /** Wrapper for Glib::Dispatcher that accepts bound functors.
     *
     * To execute a callback on the GTK+ main thread, bind the
     * parameters to Functor fn and then call send(fn).
     */
    class CallbackDispatcher {
    public:
        typedef std::function<void ()> Callback;

        CallbackDispatcher(const CallbackDispatcher&) = delete;
        CallbackDispatcher& operator=(const CallbackDispatcher&) = delete;

        CallbackDispatcher() {
            m_dispatcher.connect(sigc::mem_fun(*this,
                                               &CallbackDispatcher::on_dispatch));
        };

        /** Sends a 0 argument functor to be executed on the GTK+ main
         * thread.
         *
         * A 0 argument functor can be constructed from any functor
         * with std::bind, or a lambda may be used:
         * callback_dispatcher.send( [&label] {
         *     label.set_text("Processing 50% complete"); } );
         */
        template<typename Fn>
        void send(Fn&& fn) {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_msgs.push(std::forward<Fn>(fn));
            m_dispatcher();
        }

    private:
        std::queue<Callback> m_msgs;
        mutable std::mutex   m_mutex;
        Glib::Dispatcher     m_dispatcher;

        void on_dispatch() {
            std::lock_guard<std::mutex> lock(m_mutex);
            while (!m_msgs.empty()) {
                m_msgs.front()();
                m_msgs.pop();
            }
        }
    };
}
