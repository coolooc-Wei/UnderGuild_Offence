### 分類

| 標籤 | 優先級 | 定義與使用時機 | 實際範例 |
| --- | --- | --- | --- |
| TODO | 低 (排程處理) | 待辦事項 / 待補齊實作：記錄計畫中但尚未實作的邏輯，或已知要擴充的功能。若先寫了尚未定義的類別且不影響編譯，也歸類於此。 | ``` // TODO: 實作購物車的折扣碼計算邏輯``` |
| FIXME | 最高 (需立即處理) | 已知錯誤 / 待修復：已知會造成 Bug、報錯、中斷執行或影響編譯的程式碼。在 Code Review 時應優先被解決。 | ``` // FIXME: 這裡遇到 Null 時會導致 App 閃退，需加上防呆機制``` |
| XXX | 中 (擇期重構) | 待重構 / 壞味道：程式碼勉強能運作，但寫法很糟、維護性極差（Dirty Code）。提醒未來的自己或同事這裡有隱患。 | ``` // XXX: 暫時用硬編碼(Hardcode)寫死金鑰，未來應改接環境變數``` |
| HACK | 中 (治標不治本) | 暫時性解法 / 粗暴解法：為了繞過某個特定環境限制、奇怪的 Bug，而寫下的不合理補丁或 Workaround。 | ``` // HACK: 為了繞過 iOS Safari 的捲動 Bug，強制加上 1px 的高度``` |
| OPTIMIZE | 低 (行有餘力再做) | 效能最佳化：邏輯正確且無 Bug，但未來可以進一步提升執行速度、減少記憶體消耗。 | ``` // OPTIMIZE: 目前使用雙層迴圈比對，資料量大時應改用 Hash Map``` |

### 推薦Extertension: 
[Todo Tree](https://github.com/Gruntfuggly/todo-tree)

### 排版規則
第一行 ``/* TAG: 標題``：直接寫入主旨。
文字行 ``** 文字內容`` ：用於描述原因、背景或步驟，對齊第一行。
程式碼行 `` > 程式碼`` ：用於放置被註解掉的程式碼草稿，或是預期要寫的 API 呼叫。

``` cpp
/* TODO: 實作 Time 類別與日期轉換
**       這裡需要處理從資料庫抓取到的 timestamp 並轉換為 ISO8601。
**       考慮到小型專案規模，暫不導入重型時間庫。
 > Time dt;
 > dt.fromTimestamp(raw_data);
 > return dt.toISOString();
*/
```

### Settings.json
``` json
{
// Todo Tree settings
    "todo-tree.general.tags": [
        "TODO",
        "FIXME",
        "XXX",
        "HACK",
        "OPTIMIZE",
        "PERF"
    ],
    "todo-tree.general.tagGroups": {
        "OPTIMIZE": [
            "OPTIMIZE",
            "PERF"
        ]
    },
    "todo-tree.highlights.defaultHighlight": {
        "type": "tag",
        "opacity": 80,
        "borderRadius": "3px",
        "gutterIcon": true
    },
    "todo-tree.highlights.customHighlight": {
        "TODO": {
            "icon": "check",
            "foreground": "#ffffff",
            "background": "#5c98ff",
            "iconColour": "#5c98ff"
        },
        "FIXME": {
            "icon": "alert",
            "foreground": "#ffffff",
            "background": "#ff3333",
            "iconColour": "#ff3333",
            "type": "text",
            "fontWeight": "bold"
        },
        "XXX": {
            "icon": "unverified",
            "foreground": "#ffffff",
            "background": "#d75cff",
            "iconColour": "#d75cff"
        },
        "HACK": {
            "icon": "tools",
            "foreground": "#ffffff",
            "background": "#ff9d5c",
            "iconColour": "#ff9d5c"
        },
        "OPTIMIZE": {
            "icon": "light-bulb",
            "foreground": "#1a1a1a",
            "background": "#5cff9d",
            "iconColour": "#5cff9d"
        }
    }
},
"todo-tree.regex.regex": "(/\\*|\\*\\*|>)\\s*($TAGS)",
```
