#pragma once

#include <iostream>
#include <format>
#include <mutex>

namespace debug
{
    extern std::mutex mtx;

    template <typename... Args>
    void log(std::format_string<Args...> fmt, Args &&...args)
    {
        std::scoped_lock lock(mtx);
        const char *prefix = "[LOG] ";
        std::cout << std::format("{}{}", prefix, std::format(fmt, std::forward<Args>(args)...)) << std::endl;
    }

    template <typename... Args>
    void panic(std::format_string<Args...> fmt, Args &&...args)
    {
        std::scoped_lock lock(mtx);
        const char *prefix = "[PANIC] ";
        std::cerr << std::format("{}{}", prefix, std::format(fmt, std::forward<Args>(args)...)) << std::endl;
        std::exit(1);
    }
} // namespace debug
