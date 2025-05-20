///
/// @file observer_ptr.hpp
/// @author geffevil
/// @brief A custom smart pointer allowing to have one owner of the data and several readers of that data
///
#pragma once

#include <cstring>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace utils {

    /* Forward declarations (otherwise I would die) */
    template <typename T> class observer_ptr;
    template <typename T> class observable_ptr;   

    template <typename D, typename S> observable_ptr<D> observer_cast(observable_ptr<S>&& ptr);
    template <typename D, typename S> observer_ptr<D> observer_cast(observer_ptr<S>&& ptr);

    namespace _internal {

        /// @brief Internal state of the @c observable_ptr and all its observers
        /// @private
        struct observable_state {
            size_t active_observers;    ///< Number of active observers of this observable pointer
            bool ptr_valid;             ///< Flag, if upstream observable pointer is still valid
        };
    }

    /// @brief A smart pointer that has the ablility to spawn "read-only" observers
    ///
    /// This class satisfies the requirements of @a MoveConstructible and @a MoveAssignable, however it is neither @a CopyConstructible nor @a CopyAssignable
    /// @warning This implementation is not thread-safe! 
    /// @see observer_ptr, observer_cast, make_observable
    template <typename T> 
    class observable_ptr {

        /* Assert that only copyable Ts are used to create observable ptr */
        static_assert(std::is_copy_constructible<T>::value);

        /* So this class does not feel lonely <3 */   
        friend class observer_ptr<T>;
        template<typename D, typename S> friend observable_ptr<D> observer_cast(observable_ptr<S>&& ptr);

        public:
            constexpr observable_ptr() noexcept
                : m_data(nullptr), m_state(new _internal::observable_state{0, false}) {}

            constexpr observable_ptr(T* data) noexcept
                : m_data(data), m_state(new _internal::observable_state{0, true}) {}

            constexpr observable_ptr(observable_ptr<T>&& other) noexcept
                : m_data(other.m_data), m_state(other.m_state) {

                /* Invalidate the old object */
                other.m_state = nullptr;
                other.m_data = nullptr;
            }

            ~observable_ptr() noexcept {

                /* If state is valid, delete it */
                if (m_state != nullptr) {

                    /* Mark _data as invalid */
                    m_state->ptr_valid = false;

                    /* If there are no active observers, delete the _state struct */
                    if (m_state->active_observers == 0)
                        delete m_state;
                }
            
                /* Delete the underlying data */
                delete m_data;
            }

            constexpr observable_ptr<T>& operator=(observable_ptr<T>&& other) noexcept {
            
                /* Delete old _state, since new one is going to get moved in */
                if (m_state != nullptr)
                    delete m_state;

                /* Delete old _data */ 
                delete m_data;

                /* Move over data */
                m_data = other.m_data;
                m_state = other.m_state;

                /* Invalidate the old object */
                other.m_data = nullptr;
                other.m_state = nullptr;
                return *this;
            }

            constexpr T& operator*() const { 
                if (m_data == nullptr) throw std::runtime_error("Trying to dereference an invalid pointer");
                return *m_data; 
            }               
            constexpr T* operator->() const { 
                if (m_data == nullptr) throw std::runtime_error("Trying to dereference an invalid pointer");
                return m_data; 
            }

            constexpr bool operator==(const observable_ptr<T>& other) const noexcept {
                return m_data == other.m_data;
            }

            constexpr observer_ptr<T> observer() {
                if (m_state == nullptr || !m_state->ptr_valid)
                    throw std::runtime_error("observable_ptr does not contain any valid data to be observed!");
                /* Register the observer */
                m_state->active_observers++;
                return observer_ptr<T>(m_data, m_state);
            }

        private:
            constexpr observable_ptr(T* data, _internal::observable_state* state) noexcept
                : m_data(data), m_state(state) {}

            T* m_data;
            _internal::observable_state* m_state;
        };

    /// @brief A "read-only" view of the parent @c observable_ptr
    ///
    /// Unlike @c observable_ptr, this class satisfies the requirements of @a CopyConstructible and @a CopyAssignable
    /// @warning This implementation is not thread-safe! 
    template <typename T> 
    class observer_ptr {
            
        /* So this class does not feel lonely <3 */   
        friend class observable_ptr<T>;
        template<typename D, typename S> friend observable_ptr<D> observer_cast(observable_ptr<S>&& ptr);
        template<typename D, typename S> friend observer_ptr<D> observer_cast(observer_ptr<S>&& ptr); /* For the private constructor */
            
        public:
            constexpr observer_ptr() noexcept
                : m_data(nullptr), m_state(nullptr) {}

            ~observer_ptr() noexcept { 

                /* This object was moved and has it's state taken over */
                
                if (m_state == nullptr)
                    return;
                
                /* Decrease number of active observers */
                m_state->active_observers--;
                
                /* If pointer is invalid and there are no active observers, delete _state */
                /* If the pointer is still valid, it may spawn more observers in the future */
                if (!m_state->ptr_valid && m_state->active_observers == 0)
                    delete m_state;
                
                /* No need to delete *data as this is handled by the parent observable_ptr */
            }

            constexpr observer_ptr(const observer_ptr<T>& other) noexcept
                : m_data(other.m_data), m_state(other.m_state) {

                /* As observer_ptr does not manage data, a simple copy of the pointer can be used */
                /* If state is valid, increase instance count */
                if (m_state != nullptr)
                    m_state->active_observers++;
            }
            
            constexpr observer_ptr(observer_ptr<T>&& other) noexcept
                : m_data(other.m_data), m_state(other.m_state) {

                other.m_data = nullptr;
                other.m_state = nullptr;
            }

            inline constexpr bool valid() const { return m_state != nullptr && m_state->ptr_valid; }

            constexpr observer_ptr<T>& operator=(const observer_ptr<T>& other) noexcept {
            
                m_data = other.m_data;
                m_state = other.m_state;
                if (m_state != nullptr)
                    m_state->active_observers++;
                return *this;
            }

            constexpr T& operator*() const { 
                if (!valid()) throw std::runtime_error("Trying to dereference an invalid pointer");
                return *m_data; 
            }

            constexpr T* operator->() const { 
            
                if (!valid()) throw std::runtime_error("Trying to dereference an invalid pointer");
                return m_data; 
            }

            constexpr bool operator==(const observer_ptr<T>& other) const noexcept {

                if (!valid() || !other.valid())
                    return false;

                return m_data == other.m_data;
            }

            constexpr operator bool() const noexcept { return valid(); }

        private:
            constexpr observer_ptr(T* data, _internal::observable_state* state) noexcept
                : m_data(data), m_state(state) {}

            T* m_data;
            _internal::observable_state* m_state;
    };

    template <typename D, typename S>
    observable_ptr<D> observer_cast(observable_ptr<S>&& ptr) {

        if (ptr.m_data == nullptr)
            throw std::runtime_error("Object already deleted or moved");

        D* data = dynamic_cast<D*>(ptr.m_data);  
        _internal::observable_state* state = ptr.m_state; /* This allows to keep old observers valid even after cast */

        /* Invalidate old object */
        ptr.m_data = nullptr;
        ptr.m_state = nullptr;
            
        return observable_ptr<D>(data, state);     
    }

    template <typename D, typename S>
    observer_ptr<D> observer_cast(observer_ptr<S>&& ptr) {

        if (!ptr.valid())
            throw std::runtime_error("Trying to cast an invalid observer!");

        D* data = dynamic_cast<D*>(ptr.m_data);
        _internal::observable_state* state = ptr.m_state;

        /* Invalidate old object */
        ptr.m_state = nullptr;

        return observer_ptr<D>(data, state);
    }     
        
    /// @brief Constructs an @c observable_ptr from provided arguments
    ///
    /// This function takes variable number of arguments and forwards them to construct a new instance of an object of type @c T.
    /// @return @c observable_ptr to an object of type @c T. An @c observable_ptr returned this way is @b always @b valid.
    template <typename T, typename ...Args>
    observable_ptr<T> make_observable(Args&&... args) { 
        return observable_ptr<T>(new T(
            std::forward<Args>(args)...
        ));
    }
};
