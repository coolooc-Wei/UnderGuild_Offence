#include "Core/Time.hpp"

namespace UGO::Core::Time {

    static Tick s_CurrentTick = 0; 

    Tick GetCurrentTick() { return s_CurrentTick; }

    void AdvanceTick() { s_CurrentTick++; }

}
