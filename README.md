# AMR Task Controller

本專案為 AMR（自主移動機器人）任務排程系統的簡易模擬，模擬多個 client 向 server 並發發送任務的情境，並利用 semaphore 控制共享資源的併發使用上限。

---

## 📁 專案結構

```
amr_task_controller/
├── include/            # 標頭檔（job_handler.h）
├── src/                # 程式原始碼（main.c, job_handler.c, client.c）
├── build/              # 編譯輸出檔案（server, client）
├── run_clients.sh      # 一鍵測試腳本（模擬多 client 發送任務）
├── Makefile            # 建構規則
└── README.md
```

---

## 🚀 使用說明

### 編譯程式
```bash
make
```

### 啟動 Server
```bash
./build/server
```

### 執行多個 Client（模擬任務）
```bash
./run_clients.sh
```

---

## ⚙️ Semaphore 設定

- `cargo_sem`：最多同時 3 個任務（material_delivery, intermediate_transfer）可使用。
- `qc_sem`：最多同時 1 個任務（sample_collection）可使用。

### 任務類型對應說明

| 任務名稱               | 說明                         | 所使用 semaphore  |
|-----------------------|------------------------------|------------------|
| material_delivery     | 從倉儲區搬送物料至工作站       | cargo_sem        |
| intermediate_transfer | 在不同製程單元間搬運半成品     | cargo_sem        |
| sample_collection     | 從指定感測點採集樣本          | qc_sem           |

### 任務處理流程

> **說明**：目前任務處理過程中使用 `sleep()` 函數模擬機器人任務執行的時間，這是一種暫時性的做法，僅用於展示 semaphore 控制排程的效果。實際應用中，可擴充為更真實的任務處理邏輯，例如：
>
> - 整合感測器資料（如 AMR 移動位置、任務狀態回報）
> - 模擬搬運耗時與距離依據的行為時間
> - 使用任務狀態列舉與事件觸發機制（event-driven simulation）
>
> 本專案目前不限制任務種類與任務數量，僅展示 semaphore 控制與併發機制，供學術或概念驗證用途。

---

## 📎 未來可擴充方向
- 加入任務優先級與排程策略
- 跨機部署支援：目前為本機端測試架構，後續可調整為支援多主機分佈式連線。
- 容器化（Docker）：未來可將 server 與 client 包裝為獨立容器，透過 docker-compose 控制部署與測試流程。
- 前端 UI 控制台：建立網頁前端，觀察各任務執行流程、即時資源狀態與排隊狀況。
