#include "App.hpp"

#include "Core/Context.hpp"

int main(int, char**) {
    auto context = Core::Context::GetInstance();

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
