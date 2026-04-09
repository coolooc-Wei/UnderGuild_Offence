#include "Scene/Weapon.hpp"
#include "Scene/StatusEffect.hpp"


#include <nlohmann/json.hpp>
#include <fstream>
#include <stdexcept>

namespace UGO::Scene {

std::unordered_map<std::string, WeaponData> Weapon::s_ParseWeaponsJson() {
    std::unordered_map<std::string, WeaponData> cache;

    const std::string path = std::string(RESOURCE_DIR) + "/weapons.json";
    std::ifstream file(path);
    if (!file.is_open()) {
        LOG_ERROR("Weapon: cannot open {}", path);
        return cache;
    }

    nlohmann::json j;
    file >> j;

    static const std::unordered_map<std::string, WeaponType> typeMap = {
        {"Melee",  WeaponType::Melee},
        {"Ranged", WeaponType::Ranged},
        {"Magic",  WeaponType::Magic},
    };

    for (const auto& item : j.at("weapons")) {
        WeaponData data;
        data.id           = item.at("id").get<std::string>();
        data.name         = item.at("name").get<std::string>();
        data.attackPower  = item.at("attackPower").get<float>();
        data.cooldownTime = item.at("cooldownTime").get<float>();
        data.imagePath    = item.at("imagePath").get<std::string>();

        const std::string typeStr = item.at("type").get<std::string>();
        auto it = typeMap.find(typeStr);
        data.type = (it != typeMap.end()) ? it->second : WeaponType::Melee;

        cache.emplace(data.id, std::move(data));
    }

    LOG_INFO("Weapon: loaded {} weapon(s) from JSON", cache.size());
    return cache;
}


// Construction
Weapon::Weapon() = default;
Weapon::~Weapon() = default;

void Weapon::Equip(const std::string& weaponId) {
    static auto s_cache = s_ParseWeaponsJson();

    auto it = s_cache.find(weaponId);
    if (it == s_cache.end()) {
        LOG_ERROR("Weapon::Equip — unknown weapon id '{}'", weaponId);
        return;
    }

    m_data = it->second;// copy into Weapon (by value)
    m_currentCooldown = 0.0f;
    m_equipped = true;

    const std::string imgPath = std::string(RESOURCE_DIR) + "/" + m_data.imagePath;
    m_Image = std::make_shared<Util::Image>(imgPath);

    LOG_INFO("Weapon::Equip — '{}' equipped (ATK={}, CD={}s)",
             m_data.name, m_data.attackPower, m_data.cooldownTime);
}

/*TODO: 
    Delete some setters and getters after confirmation
*/
//Getters
std::string Weapon::GetName() const { return m_data.name; }
WeaponType Weapon::GetType() const { return m_data.type; }
HpValue Weapon::GetAttackPower() const { return m_data.attackPower; }
Core::Time::TimeStep Weapon::GetCooldownTime() const { return m_data.cooldownTime; }

//Setters
void Weapon::SetName(const std::string& name) { m_data.name = name; }
void Weapon::SetType(WeaponType type) { m_data.type = type; }
void Weapon::SetAttackPower(HpValue v) { assert(v >= 0); m_data.attackPower = v; }
void Weapon::SetCooldownTime(Core::Time::TimeStep cd) { assert(cd >= 0); m_data.cooldownTime = cd; }


//Events
void Weapon::Attack(HpValue ownerEffAtk) {
    if (!m_equipped) return;
    HpValue total = ownerEffAtk * m_data.attackPower;
    LOG_INFO("Weapon '{}' attacks! Total ATK: {}", m_data.name, total);
}

void Weapon::Update(const Core::WorldPosition& ownerPos, HpValue ownerEffAtk) {
    if (!m_equipped) return;
    (void)ownerPos; 

    const float dt = Util::Time::GetDeltaTimeMs() / 1000.0f;
    m_currentCooldown -= dt;
    if (m_currentCooldown <= 0.0f) {
        m_currentCooldown = m_data.cooldownTime; 
        Attack(ownerEffAtk);
    }
}

/* TODO: Display m_Image via Renderer or GameObject mechanism later
Currently, PTSD Util::Image needs to be bound to a GameObject to render; this will be integrated later. */
void Weapon::OnDraw(const Core::WorldPosition& ownerPos) {
    if (!m_equipped || !m_Image) return;
    (void)ownerPos;
}

} // namespace UGO::Scene
