#include "UI/PauseMapUI.hpp"

#include "System/LevelSystem.hpp"
#include "Core/Hash.hpp"
#include <glm/gtc/constants.hpp>  // glm::half_pi

namespace UGO::UI {

// ── 建構子 ─────────────────────────────────────────────────────────────────
PauseMapUI::PauseMapUI(Util::Renderer& root, System::LevelSystem& levelSystem)
: m_Root(root), m_LevelSystem(levelSystem)
{
    // 整體背景遮罩（固定元素，只建立一次）
    m_Background = std::make_shared<Util::GameObject>();
    auto bgImg = std::make_shared<Util::Image>(PATH_BG);
    m_Background->SetDrawable(bgImg);
    m_Background->m_Transform.translation = {0.0f, 0.0f};
    // 縮放至全螢幕（1280x720）
    glm::vec2 bgSize = bgImg->GetSize();
    if (bgSize.x > 0 && bgSize.y > 0) {
        m_Background->m_Transform.scale = { 640.0f / bgSize.x, 360.0f / bgSize.y };
    }
    m_Background->SetZIndex(MAP_Z);
    m_Background->SetVisible(false);
    m_Root.AddChild(m_Background);
}

// ── 私有輔助方法 ────────────────────────────────────────────────────────────

std::shared_ptr<Scene::BasicObject> PauseMapUI::MakeIcon(
    const char* path, glm::vec2 pos, glm::vec2 size, float zIndex, float rotation
) {
    auto obj = std::make_shared<Scene::BasicObject>();
    obj->SetImage(path);
    obj->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    obj->SetSize(size.x, size.y);
    obj->GetGameObject()->m_Transform.translation = { pos.x, pos.y };
    obj->GetGameObject()->SetZIndex(zIndex);
    obj->SetRotation(rotation);
    obj->GetGameObject()->SetVisible(false);
    m_Root.AddChild(obj->GetGameObject());
    return obj;
}

void PauseMapUI::BuildRoomBlock(
    const Core::Map::RoomNode& node, glm::vec2 worldPos, bool isCurrent
) {
    RoomBlock block;

    // 底板：一般/起點 → Bt_12_1；特殊/首領 → Bt_12
    const char* basePath =
        (node.roomType == Core::Map::RoomType::Normal)
        ? PATH_ROOM_BASE
        : PATH_ROOM_SPEC;
    block.m_Base = MakeIcon(basePath, worldPos, {ROOM_SIZE, ROOM_SIZE}, ROOM_Z);

    // 疊加：首領圖示
    if (node.roomType == Core::Map::RoomType::Boss) {
        block.m_Boss = MakeIcon(PATH_DUNGEON, worldPos, {ROOM_SIZE, ROOM_SIZE}, ICON_Z);
    }

    // 疊加：通關打勾
    if (node.isCleared) {
        block.m_Check = MakeIcon(PATH_CHECK, worldPos, {ROOM_SIZE * 0.5f, ROOM_SIZE * 0.5f}, ICON_Z);
    }

    // 疊加：英雄位置
    if (isCurrent) {
        block.m_Player = MakeIcon(PATH_PLAYER, worldPos, {ROOM_SIZE, ROOM_SIZE}, ICON_Z);
    }

    m_RoomBlocks.push_back(std::move(block));
}

void PauseMapUI::BuildConnections(
    const std::vector<Core::Map::RoomNode>& layout,
    const std::unordered_map<Core::Map::MapCoord, glm::vec2, Core::Ivec2Hash>& posMap
) {
    // 只掃描 (+1,0) 和 (0,+1) 兩個方向，避免重複建立連結線
    const Core::Map::MapCoord dirs[2] = { {1, 0}, {0, 1} };

    for (const auto& room : layout) {
        for (const auto& dir : dirs) {
            Core::Map::MapCoord neighborCoord = room.mapPos + dir;
            auto neighborIt = posMap.find(neighborCoord);
            if (neighborIt == posMap.end()) { continue; }

            // 計算連結線的中心位置與方向
            glm::vec2 from = posMap.at(room.mapPos);
            glm::vec2 to   = neighborIt->second;
            glm::vec2 mid  = (from + to) * 0.5f;

            // 橫向：rotation = 0；縱向：旋轉 pi/2 弧度（框架 Transform::rotation 單位為弧度）
            float rotation = (dir.x != 0) ? 0.0f : glm::half_pi<float>();

            ConnectionLine conn;
            // 連結線尺寸：長度為中心到中心間距（ROOM_STRIDE），寬度為間距（ROOM_GAP）
            // Z-Index 低於房間底板，流出到房間結構內部的小段會被底板自然遮蓋
            conn.m_Line = MakeIcon(PATH_CONN, mid, {ROOM_STRIDE, ROOM_GAP}, CONN_Z, rotation);
            m_ConnectionLines.push_back(std::move(conn));
        }
    }
}

// ── 公開介面 ────────────────────────────────────────────────────────────────

void PauseMapUI::UpdateMap() {
    // 1. 隱藏並清理所有動態元素
    Hide();
    for (auto& block : m_RoomBlocks) {
        if (block.m_Base)   { m_Root.RemoveChild(block.m_Base->GetGameObject()); }
        if (block.m_Boss)   { m_Root.RemoveChild(block.m_Boss->GetGameObject()); }
        if (block.m_Check)  { m_Root.RemoveChild(block.m_Check->GetGameObject()); }
        if (block.m_Player) { m_Root.RemoveChild(block.m_Player->GetGameObject()); }
    }
    for (auto& conn : m_ConnectionLines) {
        if (conn.m_Line) { m_Root.RemoveChild(conn.m_Line->GetGameObject()); }
    }
    m_RoomBlocks.clear();
    m_ConnectionLines.clear();

    // 2. 取得佈局，若為空則直接返回
    const auto& layout = m_LevelSystem.GetLayout();
    if (layout.empty()) { return; }

    // 3. 計算邊界框
    int minX = layout[0].mapPos.x, maxX = layout[0].mapPos.x;
    int minY = layout[0].mapPos.y, maxY = layout[0].mapPos.y;
    for (const auto& room : layout) {
        if (room.mapPos.x < minX) { minX = room.mapPos.x; }
        if (room.mapPos.x > maxX) { maxX = room.mapPos.x; }
        if (room.mapPos.y < minY) { minY = room.mapPos.y; }
        if (room.mapPos.y > maxY) { maxY = room.mapPos.y; }
    }

    // 4. 計算置中 offset（PTSD 框架原點在螢幕中心）
    float totalW = (maxX - minX) * ROOM_STRIDE;
    float totalH = (maxY - minY) * ROOM_STRIDE;
    glm::vec2 offset = { -totalW * 0.5f, -totalH * 0.5f };

    // 5. 建立 mapPos → worldPos 的映射表（供 BuildConnections 使用）
    std::unordered_map<Core::Map::MapCoord, glm::vec2, Core::Ivec2Hash> posMap;
    posMap.reserve(layout.size());
    for (const auto& room : layout) {
        glm::vec2 worldPos = {
            offset.x + (room.mapPos.x - minX) * ROOM_STRIDE,
            offset.y + (room.mapPos.y - minY) * ROOM_STRIDE
        };
        posMap[room.mapPos] = worldPos;
    }

    // 6. 建立房間格子
    const Core::Map::RoomNode* currentRoom = m_LevelSystem.GetCurrentRoomPtr();
    for (const auto& room : layout) {
        bool isCurrent = (currentRoom && room.mapPos == currentRoom->mapPos);
        BuildRoomBlock(room, posMap[room.mapPos], isCurrent);
    }

    // 7. 建立連結線
    BuildConnections(layout, posMap);
}

void PauseMapUI::Show() {
    if (m_Background) { m_Background->SetVisible(true); }
    for (auto& block : m_RoomBlocks) {
        if (block.m_Base)   { block.m_Base->GetGameObject()->SetVisible(true); }
        if (block.m_Boss)   { block.m_Boss->GetGameObject()->SetVisible(true); }
        if (block.m_Check)  { block.m_Check->GetGameObject()->SetVisible(true); }
        if (block.m_Player) { block.m_Player->GetGameObject()->SetVisible(true); }
    }
    for (auto& conn : m_ConnectionLines) {
        if (conn.m_Line) { conn.m_Line->GetGameObject()->SetVisible(true); }
    }
}

void PauseMapUI::Hide() {
    if (m_Background) { m_Background->SetVisible(false); }
    for (auto& block : m_RoomBlocks) {
        if (block.m_Base)   { block.m_Base->GetGameObject()->SetVisible(false); }
        if (block.m_Boss)   { block.m_Boss->GetGameObject()->SetVisible(false); }
        if (block.m_Check)  { block.m_Check->GetGameObject()->SetVisible(false); }
        if (block.m_Player) { block.m_Player->GetGameObject()->SetVisible(false); }
    }
    for (auto& conn : m_ConnectionLines) {
        if (conn.m_Line) { conn.m_Line->GetGameObject()->SetVisible(false); }
    }
}

} // namespace UGO::UI
