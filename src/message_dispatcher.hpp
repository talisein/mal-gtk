#include <glibmm/dispatcher.h>
#include <queue>
#include <thread>
#include <mutex>

namespace MAL {

    /**
     *  Wrapper for a Glib::Dispatcher that provides a message queue.
     *
     *  Must be used with Glib::Dispatcher constraints (instantiated
     *  and deleted in receiving thread which must have a
     *  GMainContext).
     */
    template <typename T>
    class MessageDispatcher {
    public:
        MessageDispatcher() = default;
        MessageDispatcher(const MessageDispatcher&) = delete;
        MessageDispatcher& operator=(const MessageDispatcher&) = delete;

        template <class U>
        void operator()(U&& u) {
            std::lock_guard<std::mutex> lock(mutex);
            msgs.push(std::forward<U>(u));
            dispatcher();
        }

        template <typename Void_slot>
        sigc::connection connect(Void_slot&& void_slot) {
            return dispatcher.connect(std::forward<Void_slot>(void_slot));
        }

        typename std::queue<T>::const_reference front() const {
            std::lock_guard<std::mutex> lock(mutex);
            return msgs.front();
        }

        typename std::queue<T>::reference front() {
            std::lock_guard<std::mutex> lock(mutex);
            return msgs.front();
        }

        void pop() {
            std::lock_guard<std::mutex> lock(mutex);
            msgs.pop();
        }

        bool empty() const {
            std::lock_guard<std::mutex> lock(mutex);
            return msgs.empty();
        }
        
    private:
        std::queue<T>      msgs;
        mutable std::mutex mutex;
        Glib::Dispatcher   dispatcher;
    };

}
