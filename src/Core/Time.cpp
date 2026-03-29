#include "Core/Time.hpp"

namespace UGO::Core::Time {

    // GLOBAL VARIABLES
    TimeStep g_DeltaTime = 1.0f;


    // FUNCTIONS
    TimeStep GetDeltaTime() { return g_DeltaTime; }

}
