export module Helpers:Logger;

import std;
import SDL3;
import :Flags;

using namespace std::literals;

//Class for logging errors and messages to files and console
export class Logger
{
public:
    Logger() = default;

    [[nodiscard]] bool init()
    {
        infoLog.open(infoLogFileName.data(), std::ios::binary | std::ios::out | std::ios::trunc);
        errorLog.open(errorLogFileName.data(), std::ios::binary | std::ios::out | std::ios::trunc);
        if (!infoLog || !errorLog)
        {
            logError("Couldn't create log file, check if game folder needs admin permissions"sv);
            return false;
        }

        return true;
    }

    template<class... Args>
    void logError(std::format_string<Args...> fmt, Args&&... args)
    {
        std::array<char, 2048> errorMessageArr{};
        auto stackTrace = std::stacktrace::current();

        std::format_to_n(errorMessageArr.data(), errorMessageArr.size() - 1, fmt, std::forward<Args>(args)...);
        std::string_view errorMessage = errorMessageArr.data();

        std::println(errorLog, "Error: {}\nStacktrace:\n{}", errorMessage, stackTrace);

        if constexpr (isDebugBuild)
            std::println(std::cerr, "Error: {}\nStacktrace:\n{}", errorMessage, stackTrace);

        std::array<char, 2048 + 128> popupMessage{};
        std::format_to_n(popupMessage.data(), popupMessage.size() - 1, "{}\nCheck the error log for details. Esc to exit", errorMessage);
        displayErrorMessage(popupMessage.data());
    }

    template<class... Args>
    void logInfo(std::format_string<Args...> fmt, Args&&... args)
    {
        std::println(infoLog, fmt, std::forward<Args>(args)...);

        if constexpr (isDebugBuild)
            std::println(std::cout, fmt, std::forward<Args>(args)...);
    }

    void extraAssert(bool condition, std::string_view message)
    {
        if constexpr (useExtraAsserts)
        {
            if (condition)
                return;

            logError("{}", message);
            std::abort();
        }
    }

private:
    static void displayErrorMessage(char const* message)
    {
        SDL_ShowSimpleMessageBox(sdlMessageboxError, "Application Error", message, nullptr);
    }

    std::ofstream infoLog;
    std::ofstream errorLog;

    static constexpr auto infoLogFileName{"infoLog.txt"sv};
    static constexpr auto errorLogFileName{"errorLog.txt"sv};
};

export inline Logger logger;