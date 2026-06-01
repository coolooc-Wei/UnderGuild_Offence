#include "App.hpp"

#include "Core/Context.hpp"
#include <filesystem>
#include <spdlog/sinks/basic_file_sink.h>

int main(int, char**) {
    auto context = Core::Context::GetInstance();
    std::filesystem::create_directories(LOG_DIR);
    spdlog::default_logger()->sinks().push_back(
        std::make_shared<spdlog::sinks::basic_file_sink_mt>(LOG_DIR "/game.log", true)
    );

/* To toggle debug log, run `cmake -DUGO_DISABLE_LOG_DEBUG=ON .` */
#ifdef UGO_DISABLE_LOG_DEBUG
    Util::Logger::SetLevel(Util::Logger::Level::INFO);
#endif

    UGO::App app;

    while (!context->GetExit()) {
        switch (app.GetCurrentState()) {
            case UGO::App::State::START: {
                app.Start();
                break;
            }
            case UGO::App::State::UPDATE: {
                app.Update();
                break;
            }
            case UGO::App::State::END: {
                app.End();
                context->SetExit(true);
                break;
            }
            default: { break; }
        }
        context->Update();
    }
    return 0;
}
