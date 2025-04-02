#pragma once


#include <cstring>
#include <iostream>
#include <new>
#include <stdexcept>
#include <utility>

namespace utils {

    /* Forward declarations (otherwise I would die) */
    template <typename T> class observer_ptr;
    template <typename T> class observable_ptr;   

    template <typename _D, typename _S> observable_ptr<_D> observer_cast(observable_ptr<_S>&& ptr);
    template <typename _D, typename _S> observer_ptr<_D> observer_cast(observer_ptr<_S>&& ptr);


    /* Declarations */
    struct _observable_state {
        size_t active_observers;
        bool ptr_valid;
    };

    /// @warning This class is not thread-safe! 
    template <typename _T> 
        class observable_ptr {

            /* So this class does not feel lonely <3 */   
            friend class observer_ptr<_T>;
            template<typename _D, typename _S> friend observable_ptr<_D> observer_cast(observable_ptr<_S>&& ptr);

            public:
                observable_ptr()
                    : _data(nullptr), _state(new _observable_state{0, false}) { std::cout << "B" << std::endl; }

                observable_ptr(_T* data)
                    : _data(data), _state(new _observable_state{0, true}) { std::cout << "C" << std::endl; }

                /* TODO: Maybe learn std::allocators? */
                observable_ptr(const observable_ptr<_T>& other)
                    : _data(nullptr), _state(new _observable_state{0, other._data != nullptr}) {
                    
                    if (other._data != nullptr) {
                        _data = static_cast<_T*>(::operator new(sizeof(*other._data)));
                        std::memcpy(_data, other._data, sizeof(*other._data));
                    }    
                }

                observable_ptr(observable_ptr<_T>&& other) noexcept
                    : _data(other._data), _state(other._state) {

                    /* Invalidate the old object */
                    other._state = nullptr;
                    other._data = nullptr;
                }

                ~observable_ptr() noexcept {

                    /* If state is valid, delete it */
                    if (_state != nullptr) {
                        /* Mark _data as invalid */
                        _state->ptr_valid = false;

                        /* If there are no active observers, delete the _state struct */
                        if (_state->active_observers == 0)
                            delete _state;
                    }

                    /* Delete the underlying data */
                    delete _data;
                }

                constexpr observable_ptr<_T>& operator=(const observable_ptr<_T>& other) {

                    /* Delete old _state and establish new one */
                    if (_state != nullptr) {
                    
                        /* There are active observers. Just alloc new state and leave management */
                        /* of the old state to the observers */
                        if (_state->active_observers > 0)
                            _state = new _observable_state {0, other._data != nullptr};

                        /* Otherwise, just overwrite the old one */
                        else
                            *_state = {0, other._data != nullptr};
                    }
                    else _state = new _observable_state {0, other._data != nullptr}; /* No state exists or the object has been moved */

                    /* Delete old _data */ 
                    delete _data;
                    _data = nullptr;

                    /* Since this is a copy, observers are not carried over */

                    /* Copy data */
                    if (other._data != nullptr) {
                        _data = static_cast<_T*>(::operator new(sizeof(*other._data)));
                        std::memcpy(_data, other._data, sizeof(*other._data));
                    }

                    return *this;
                }

                constexpr observable_ptr<_T>& operator=(observable_ptr<_T>&& other) {
                
                    /* Delete old _state, since new one is going to get moved in */
                    if (_state != nullptr)
                        delete _state;

                    /* Delete old _data */ 
                    delete _data;

                    /* Move over data */
                    _data = other._data;
                    _state = other._state;

                    /* Invalidate the old object */
                    other._data = nullptr;
                    other._state = nullptr;
                    return *this;
                }

                _T& operator*() { 

                    if (_data == nullptr) throw std::runtime_error("Trying to dereference an invalid pointer");
                    return *_data; 
                }               

                _T* operator->() { 

                    if (_data == nullptr) throw std::runtime_error("Trying to dereference an invalid pointer");
                    return _data; 
                }

                constexpr observer_ptr<_T> observer() {

                    if (_state == nullptr || !_state->ptr_valid)
                        throw std::runtime_error("observable_ptr does not contain any valid data to be observed!");

                    /* Register the observer */
                    _state->active_observers++;
                    return observer_ptr<_T>(_data, _state);
                }

            private:
                constexpr observable_ptr(_T* data, _observable_state* state)
                    : _data(data), _state(state) {}

                _T* _data;
                _observable_state* _state;
        };

    template <typename _T> 
        class observer_ptr {
            
            /* So this class does not feel lonely <3 */   
            friend class observable_ptr<_T>;
            template<typename _D, typename _S> friend observable_ptr<_D> observer_cast(observable_ptr<_S>&& ptr);
            template<typename _D, typename _S> friend observer_ptr<_D> observer_cast(observer_ptr<_S>&& ptr); /* For the private constructor */
            
            public:
                ~observer_ptr() { 

                    /* This object was moved and has it's state taken over */
                    if (_state == nullptr)
                        return;

                    /* Decrease number of active observers */
                    _state->active_observers--;

                    /* If pointer is invalid and there are no active observers, delete _state */
                    /* If the pointer is still valid, it may spawn more observers in the future */
                    if (!_state->ptr_valid && _state->active_observers == 0)
                        delete _state;

                    /* No need to delete *data as this is handled by the parent observable_ptr */
                }

                observer_ptr(const observer_ptr<_T>& other)
                    : _data(other._data), _state(other._state) {

                    /* As observer_ptr does not manage data, a simple copy of the pointer can be used */
                    /* If state is valid, increase instance count */
                    if (_state != nullptr)
                        _state->active_observers++;
                }
                
                observer_ptr(observer_ptr<_T>&& other)
                    : _data(other._data), _state(other._state) {

                    other._data = nullptr;
                    other._state = nullptr;
                }

                inline bool valid() const { return _state != nullptr && _state->ptr_valid; }

                _T& operator*() { 

                    if (!valid()) throw std::runtime_error("Trying to dereference an invalid pointer");
                    return *_data; 
                }
    
                _T* operator->() { 
                
                    if (!valid()) throw std::runtime_error("Trying to dereference an invalid pointer");
                    return _data; 
                }

            private:
                constexpr observer_ptr(_T* data, _observable_state* state) 
                    : _data(data), _state(state) {}

                _T* _data;
                _observable_state* _state;
        };

    template <typename _D, typename _S>
        observable_ptr<_D> observer_cast(observable_ptr<_S>&& ptr) {

            std::cout << std::hex << ptr._data << std::endl;
            std::cout << std::hex << ptr._state << std::endl;


            if (ptr._data == nullptr)
                throw std::runtime_error("Object already deleted or moved");

            _D* data = dynamic_cast<_D*>(ptr._data);  
            _observable_state* state = ptr._state; /* This allows to keep old observers valid even after cast */

            /* Invalidate old object */
            ptr._data = nullptr;
            ptr._state = nullptr;
            
            return observable_ptr<_D>(data, state);     
        }

    template <typename _D, typename _S>
        observer_ptr<_D> observer_cast(observer_ptr<_S>&& ptr) {

            if (!ptr.valid())
                throw std::runtime_error("Trying to cast an invalid observer!");

            _D* data = dynamic_cast<_D*>(ptr._data);
            _observable_state* state = ptr._state;

            /* Invalidate old object */
            ptr._state = nullptr;

            return observer_ptr<_D>(data, state);
        }     
        
    template <typename _T, typename ..._Args>
        observable_ptr<_T> make_observable(_Args... args) { 
            return observable_ptr<_T>(new _T(
                std::forward<_Args>(args)...
            ));
        }
};
