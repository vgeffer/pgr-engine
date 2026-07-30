#pragma once
#include <common/common.hpp>

#include "backend/backend_types.hpp"
#include "common/os/dynamic_loader.hpp"

namespace pgreng::backends {

    class backend {
        public:
            backend() = default;
            backend(const std::string& lib_path) {

                /* manually link lib_init and lib_get_func_table */
                common::os::dynamic_loader loader;

                /* TODO: keep updated! */
                void (*lib_init)(hbackend&) = loader.func_addr<void, hbackend&>("lib_init");
                void** (*lib_get_func_table)(const hbackend&) = loader.func_addr<void**, const hbackend&>("lib_get_func_table");


                lib_init(m_backend);
                m_func_table = lib_get_func_table(m_backend);
                s_instance = this;
            }

            ~backend() {

                lib_destroy(m_backend);
                m_func_table = nullptr;
            }

            #define MODIFIER static
            #define FUNCTION_TABLE s_instance->m_func_table
            #define EXPORT_FUNCTION_WRAPPERS
            #include <backends/backend/backend_functions.hpp>

        private:
            inline static backend* s_instance = nullptr;
            hbackend m_backend;
            void** m_func_table = nullptr;

    };
}



