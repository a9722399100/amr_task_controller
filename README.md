# AMR Task Controller

本專案為一套模擬 AMR（自主移動機器人）任務排程系統的簡易框架，模擬多個 client 向 server 並發送任務，並透過 semaphore 控制共享資源的使用上限，具備任務優先級佇列、dispatcher、CLI 測試腳本與 ncurses UI。

---

## 📁 專案結構

```
amr_task_controller/
├── include/            # 標頭檔（job_handler.h, task_queue.h, ui_ncurses.h）
├── src/                # 程式原始碼（main.c, job_handler.c, task_queue.c, client.c, ui_ncurses.c）
├── build/              # 編譯輸出檔案（server, client）
├── assets/             # 截圖示意（ScreenShot.png）
├── run_clients.sh      # 一鍵測試腳本（模擬多個 client 發送任務）
├── Makefile            # 編譯規則
└── README.md           # 專案說明文件
```

---

## 🚀 使用說明

### 編譯程式

```bash
make
```

### 啟動 Server（含 ncurses 任務監控 UI）

```bash
./build/server
```

### 模擬多個 Client 發送任務

```bash
./run_clients.sh
```

可自訂發送方式（單一 client）：
```bash
./build/client "REQUEST job=material_delivery; from=storage; to=station1; priority=2"
```

---

## 🧠 任務排程邏輯與資源設定

- `cargo_sem`：最多允許 **3** 個 Cargo 任務（material_delivery / intermediate_transfer）同時執行。
- `qc_sem`：最多允許 **1** 個 QC 任務（sample_collection）同時執行。
- Cargo 任務會進入 **優先級佇列**，由 **dispatcher thread** 按照 priority 排序並發派執行。

### 任務類型說明

| 任務名稱               | 描述                             | 使用 semaphore |
|------------------------|----------------------------------|----------------|
| `material_delivery`     | 將物料從倉儲區送至工作站          | `cargo_sem`    |
| `intermediate_transfer` | 在製程單元間搬運半成品            | `cargo_sem`    |
| `sample_collection`     | 採集樣本至 QC 檢測站              | `qc_sem`       |

### 任務處理模擬流程

每個任務在處理過程中會使用 `sleep()` 模擬執行時間，並於終端顯示狀態紀錄與統計耗時。

---

## 🖥️ 任務狀態即時監控（UI）

Server 啟動後會同時啟動 **ncurses-based UI 面板**，可觀察 queue 中任務的：

- 任務 ID / Client 編號 / 任務敘述
- 狀態（Pending / Running / Done）
- 等待時間與執行時間

**按下 `q` 可離開面板**。

---

## ✅ 已實作功能

- [x] 多任務類型（含資源限制控制）
- [x] 優先級任務佇列與 dispatcher 管理
- [x] 多 client 模擬（腳本 or 多終端啟動）
- [x] ncurses UI 即時監控任務狀態
- [x] 跨主機連線支援（Client 可指定 IP）

---

## 🛠️ 可擴充功能（開發中）

- [ ] **容器化（Docker）**：封裝 server/client，利於部署與測試
- [ ] **前端 UI 控制台（Web-based）**：提供瀏覽器即時控制與任務查詢

---

## 📷 實際執行畫面

![AMR UI Screenshot](assets/ScreenShot.png)

---

## 🔖 License

MIT License - feel free to use or modify for academic or research purposes.
