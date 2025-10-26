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

    void logError(std::string_view message)
    {
        auto stackTrace = std::stacktrace::current();
        std::println(errorLog, "Error: {}\nStacktrace:\n{}", message, stackTrace);

        if constexpr (isDebugBuild)
            std::println(std::cerr, "Error: {}\nStacktrace:\n{}", message, stackTrace);

        std::array<char, 2048> popupMessage{};
        std::format_to_n(popupMessage.data(), popupMessage.size() - 1, "{}\nCheck the error log for details. Esc to exit", message);
        displayErrorMessage(popupMessage.data());
    }

    void logInfo(std::string_view message)
    {
        std::println(infoLog, "{}", message);

        if constexpr (isDebugBuild)
            std::println(std::cout, "{}", message);
    }

    void extraAssert(bool condition, std::string_view message)
    {
        if constexpr (useExtraAsserts)
        {
            if (condition)
                return;

            logError(message);
            std::abort();
        }
    }

private:
    static void displayErrorMessage(std::string_view message)
    {
        SDL_ShowSimpleMessageBox(sdlMessageboxError, "Application Error", message.data(), nullptr);
    }

    std::ofstream infoLog;
    std::ofstream errorLog;

    static constexpr auto infoLogFileName{"infoLog.txt"sv};
    static constexpr auto errorLogFileName{"errorLog.txt"sv};
};

export inline Logger logger;