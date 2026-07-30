/* Helper macros */
#if defined(BACKEND_IMPLEMENTATION)
    #define EXPORT_FUNCTION_PROTOTYPES
    #include <common/common.hpp>
    #include "backend_types.hpp"
    #undef BACKEND_IMPLEMENTATION
#endif


#if !defined(TARGET_OBJECT)
    #define TARGET_OBJECT
#endif

#if defined(INCLUDE_TYPES)
    #include "backend_types.hpp"
    #undef INCLUDE_TYPES
#endif



#if defined(EXPORT_PFN_TYPES)
    #define PUBLIC_BACKEND_FUNCTION(type, name, ...) typedef type(*ptr_##name)(__VA_ARGS__);

#elif defined(EXPORT_PFN_VARIABLES)
    #define PUBLIC_BACKEND_FUNCTION(type, name, ...) ptr_##name fn_##name = nullptr;

#elif defined(EXPORT_FUNCTION_PROTOTYPES) 
    #define PUBLIC_BACKEND_FUNCTION(type, name, ...) PGRAPI extern "C" type name(__VA_ARGS__);

#elif defined(EXPORT_FUNCTION_NAMES)
    #define PUBLIC_BACKEND_FUNCTION(type, name, ...) const char *const name##Name = #name; 

#elif defined(EXPORT_FUNCTION_TABLES)
    #if !defined(FUNCTION_TABLE)
        #error Function Tables not provided!
    #elif !defined(LOADER)
        #error Loader not provided!
    #endif

    #define PUBLIC_BACKEND_FUNCTION(type, name, ...) FUNCTION_TABLE[__COUNTER__] = reinterpret_cast<void*>(LOADER.func_addr<type, __VA_ARGS__>(#name));

#elif defined(EXPORT_FUNCTION_WRAPPERS)
    #if !defined(FUNCTION_TABLE)
        #error Function Tables not provided!
    #endif

    #if !defined(MODIFIER)
        #define MODIFIER
    #endif

    #define PUBLIC_BACKEND_FUNCTION(type, name, ...) template <typename ...Args> MODIFIER type name(Args... args) { return (reinterpret_cast<type(*)(__VA_ARGS__)>(FUNCTION_TABLE[__COUNTER__]))(std::forward<__VA_ARGS__>(args)...);}

#else
    #define PUBLIC_BACKEND_FUNCTION(type, name, ...)

#endif                                                             

/* Library */
PUBLIC_BACKEND_FUNCTION(void, lib_init, hbackend&, common::os::dynamic_loader&)
PUBLIC_BACKEND_FUNCTION(void, lib_destroy, hbackend&)
PUBLIC_BACKEND_FUNCTION(void, lib_get_info, backend_metadata&)
PUBLIC_BACKEND_FUNCTION(void**, lib_get_func_table, const hbackend&)

/* Features */

/* Graphics */
PUBLIC_BACKEND_FUNCTION(void, win_create, hwindow&, const char*);
PUBLIC_BACKEND_FUNCTION(void, win_destroy, hwindow&);
PUBLIC_BACKEND_FUNCTION(void, win_present, hwindow&);
PUBLIC_BACKEND_FUNCTION(bool, win_should_close, const hwindow&);
PUBLIC_BACKEND_FUNCTION(void, win_set_title, hwindow&, const std::string&);
PUBLIC_BACKEND_FUNCTION(void, win_set_param, hwindow&, window_params, void*);
PUBLIC_BACKEND_FUNCTION(bool, win_get_next_event, const hwindow&, window_event&);
PUBLIC_BACKEND_FUNCTION(void, win_swap_buffers, const hwindow&); //todo: presentation?

/* */
PUBLIC_BACKEND_FUNCTION(void, gfx_init);
PUBLIC_BACKEND_FUNCTION(void, gfx_destroy);

#undef PUBLIC_BACKEND_FUNCTION
#undef EXPORT_PFN_TYPES
#undef EXPORT_PFN_VARIABLES
#undef EXPORT_FUNCTION_PROTOTYPES
#undef EXPORT_FUNCTION_NAMES
#undef EXPORT_FUNCTION_TABLES

#undef MODIFIER
#undef FUNCTION_TABLE
#undef LOADER