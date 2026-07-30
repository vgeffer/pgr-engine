#pragma once
#include <utility>
#include <common/common.hpp>

namespace events {

    #define EVENT(...) events::event<__VA_ARGS__>
    #define TO_HANDLER(event, function) decltype(event)::handler_type::create<&function>()
    #define TO_HANDLER2(event, function) decltype(event)::handler_type::create<SELF, &function>(this)
    #define TO_HANDLER_CLASS(event, function, instance) decltype(event)::handler_type::create<std::remove_pointer<decltype(instance)>::type, &function>(instance)

    class event_handler_base {};

    template <typename T>
    class event_handler;

    template <typename Ret, typename ...Args>
    class event_handler<Ret(Args...)> {

        public:
            ~event_handler() {
                m_prev_event->next(m_next_event);
                m_next_event->prev(m_prev_event);
            }

            template<Ret(* FnPtr)(Args...)>
            static event_handler* create() {
                return new event_handler(nullptr, &non_member_wrapper<FnPtr>);
            }

            template<typename Class, Ret(Class::* FnPtr)(Args...)>
            static event_handler* create(Class* this_ptr) {
                return new event_handler(static_cast<void*>(this_ptr), &member_wrapper<Class, FnPtr>);
            }

            inline event_handler<Ret(Args...)>* next() const noexcept { return m_next_event; }
            inline void next(event_handler<Ret(Args...)>* next) { m_next_event = next; }
            inline event_handler<Ret(Args...)>* prev() const { return m_prev_event; }
            inline void prev(event_handler<Ret(Args...)>* prev) { m_prev_event = prev; }

            Ret operator()(Args ...args) { return (*m_fn_ptr)(m_this_ptr, std::forward<Args>(args)...); }
            bool operator==(event_handler<Ret(Args...)> other) {
                return m_this_ptr == other.m_this_ptr &&
                       m_fn_ptr == other.m_fn_ptr;
            }

        private:
            template<Ret(* FnPtr)(Args...)>
            static inline Ret non_member_wrapper(void* this_ptr, Args... args) {
                (void)this_ptr;
                return FnPtr(args...);
            }

            template<typename Class, Ret(Class::* FnPtr)(Args...)>
            static inline Ret member_wrapper(void* this_ptr, Args... args) {
                return (static_cast<Class*>(this_ptr)->*FnPtr)(args...);
            }

            event_handler(void* this_ptr, Ret(*fn)(void*, Args...))
                : m_this_ptr(this_ptr), m_fn_ptr(fn), m_prev_event(nullptr), m_next_event(nullptr) {}

            void* m_this_ptr;
            Ret(* m_fn_ptr)(void*, Args...);
            event_handler<Ret(Args...)>* m_prev_event, * m_next_event;
    };

    template<typename ...Args>
    class event {
        public:
            using handler_type = event_handler<void(Args...)>;

        public:
            inline void operator()(Args... args) { invoke(args...); }
            inline void operator+=(event_handler<void(Args...)>* other) {
                /* TODO: Lockless add */
                other->next(m_head);
                if (m_head) m_head->prev(other);
                m_head = other;
            }

            inline void operator-=(event_handler<void(Args...)>& other) {}

            void invoke(Args... args) {
                event_handler<void(Args...)>* walk = m_head;
                while(walk != nullptr) {
                    (*walk)(args...);
                    walk = walk->next();
                }
            }

        private:
            event_handler<void(Args...)>* m_head = nullptr;
    };
};
