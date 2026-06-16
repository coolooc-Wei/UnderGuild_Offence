#ifndef PAUSE_MAP_UI_HPP
#define PAUSE_MAP_UI_HPP

#include "UGO_pch.hpp"
#include "Core/Hash.hpp"
#include "Core/MapData.hpp"
#include "Scene/BasicObject.hpp"
#include "Util/Renderer.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

// Forward declaration
namespace UGO::System { class LevelSystem; }

namespace UGO::UI {

/**
 * @class PauseMapUI
 * @brief 在手動暫停時於畫面中央顯示圖形化的關卡地圖。
 *
 * 職責：
 * - 封裝所有地圖相關的 UI 元素（背景、房間格子、連結線）
 * - 透過 LevelSystem 的公開介面取得資料，不依賴任何其他系統
 * - 只有 App::ChangeGameState 決定何時呼叫 Show/Hide/UpdateMap
 *
 * 使用方式：
 * @code
 *   m_PauseMapUI->UpdateMap();  // 重新生成地圖（每次暫停時呼叫）
 *   m_PauseMapUI->Show();       // 顯示地圖
 *   m_PauseMapUI->Hide();       // 隱藏地圖
 * @endcode
 */
class PauseMapUI {
public:
    PauseMapUI(Util::Renderer& root, System::LevelSystem& levelSystem);
    ~PauseMapUI() = default;

    /** @brief 清理舊元素後，根據當前 LevelSystem 佈局重新生成整個地圖。 */
    void UpdateMap();

    /** @brief 顯示所有已建立的 UI 元素。 */
    void Show();

    /** @brief 隱藏所有 UI 元素。 */
    void Hide();

private:
    // ── 尺寸常數 ──────────────────────────────────────────────────
    static constexpr float ROOM_SIZE   = 42.0f;  ///< 每個房間格子的顯示尺寸（px）
    static constexpr float ROOM_GAP    = 5.0f;   ///< 相鄰格子中心點之間的額外留白（px）
    static constexpr float ROOM_STRIDE = ROOM_SIZE + ROOM_GAP; ///< 格子步長（px）
    static constexpr float MAP_Z       = 55.0f;     ///< 整體背景的 Z-Index
    static constexpr float ROOM_Z      = 56.0f;     ///< 房間底板的 Z-Index
    static constexpr float ICON_Z      = 57.0f;     ///< 疊加圖示的 Z-Index
    static constexpr float CONN_Z      = 55.5f;     ///< 連結線的 Z-Index（低於 ROOM_Z，讓房間底板自然遮蓋末端）

    // ── 資源路徑常數 ───────────────────────────────────────────────
    static constexpr const char* PATH_BG        = "../Resources/Image/card/Ui_CardBg2.png";
    static constexpr const char* PATH_ROOM_BASE = "../Resources/Image/button/Bt_12_1.png"; ///< 一般/起點房間
    static constexpr const char* PATH_ROOM_SPEC = "../Resources/Image/button/Bt_12.png";   ///< 特殊/首領房間
    static constexpr const char* PATH_DUNGEON   = "../Resources/Image/button/Bt_Dungeon.png";
    static constexpr const char* PATH_CHECK     = "../Resources/Image/button/Check.png";
    static constexpr const char* PATH_PLAYER    = "../Resources/Image/button/Event_Board_1.png";
    static constexpr const char* PATH_CONN      = "../Resources/Image/button/GuildImage_2.png";

    // ── 內部資料結構 ───────────────────────────────────────────────

    /**
     * @struct RoomBlock
     * @brief 封裝單一房間格子的所有 UI 元素。
     *
     * 每個 RoomBlock 由底板（必有）加上可選的疊加圖示組成：
     *   - m_Boss：首領圖示（僅 Boss 房間）
     *   - m_Check：通關打勾（isCleared == true）
     *   - m_Player：英雄位置標記（當前房間）
     */
    struct RoomBlock {
        std::shared_ptr<Scene::BasicObject> m_Base;   ///< 房間底板（必有）
        std::shared_ptr<Scene::BasicObject> m_Boss;   ///< 首領疊加（可選）
        std::shared_ptr<Scene::BasicObject> m_Check;  ///< 通關疊加（可選）
        std::shared_ptr<Scene::BasicObject> m_Player; ///< 英雄疊加（可選）
    };

    /**
     * @struct ConnectionLine
     * @brief 封裝單條房間連結線的 UI 元素。
     */
    struct ConnectionLine {
        std::shared_ptr<Scene::BasicObject> m_Line; ///< 連結線圖示
    };

    // ── 私有方法 ───────────────────────────────────────────────────

    /**
     * @brief 建立單一 BasicObject 圖示並加入 Renderer。
     * @param path      圖片路徑
     * @param pos       世界座標
     * @param size      顯示尺寸（寬=高）
     * @param zIndex    渲染層級
     * @param rotation  旋轉角度（度）
     * @return 建立好的 BasicObject（初始隱藏）
     */
    std::shared_ptr<Scene::BasicObject> MakeIcon(
        const char* path, glm::vec2 pos, glm::vec2 size, float zIndex, float rotation = 0.0f
    );

    /**
     * @brief 根據單一 RoomNode 建立 RoomBlock 並推入 m_RoomBlocks。
     * @param node      房間節點資料
     * @param worldPos  此格子的世界座標（已套用 offset 置中）
     * @param isCurrent 是否為英雄當前所在房間
     */
    void BuildRoomBlock(const Core::Map::RoomNode& node, glm::vec2 worldPos, bool isCurrent);

    /**
     * @brief 掃描整個佈局，對每對相鄰房間建立 ConnectionLine 並推入 m_ConnectionLines。
     * @param layout    完整房間佈局（vector of RoomNode）
     * @param layoutMap 快速查詢 mapPos → worldPos 的映射表
     */
    void BuildConnections(
        const std::vector<Core::Map::RoomNode>& layout,
        const std::unordered_map<Core::Map::MapCoord, glm::vec2, Core::Ivec2Hash>& layoutMap
    );

    // ── 成員變數 ───────────────────────────────────────────────────
    Util::Renderer&         m_Root;
    System::LevelSystem&    m_LevelSystem;

    std::shared_ptr<Util::GameObject> m_Background;        ///< 整體背景遮罩
    std::vector<RoomBlock>        m_RoomBlocks;        ///< 所有房間格子（動態，每次 UpdateMap 重建）
    std::vector<ConnectionLine>   m_ConnectionLines;   ///< 所有連結線（動態，每次 UpdateMap 重建）
};

} // namespace UGO::UI

#endif // PAUSE_MAP_UI_HPP
