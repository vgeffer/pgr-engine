#pragma once
#include <iostream>
#include <ostream>
#include <streambuf>

class logger {

    public:
        struct log_stream_t : public std::streambuf {
            public:
                log_stream_t();

                virtual int sync() {
                    return 0;
                }
        };


    private:
        inline static logger* s_instance = nullptr;


    public:
        inline static std::ostream& error    = std::cerr;///std::ostream(log_stream_t());
        inline static std::ostream& warn     = std::cerr;///std::ostream(log_stream_t());
        inline static std::ostream& log      = std::cerr;///std::ostream(log_stream_t());
        inline static std::ostream& info     = std::cerr;///std::ostream(log_stream_t());
        inline static std::ostream& debug    = std::cerr;///std::ostream(log_stream_t());
        
};