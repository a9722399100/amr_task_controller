# AMR Task Controller

本專案為 AMR（自主移動機器人）任務排程系統的簡易模擬，模擬多個 client 向 server 並發發送任務的情境，並利用 semaphore 控制共享資源的併發使用上限。

---

## 📁 專案結構

```
amr_task_controller/
├── include/            # 標頭檔（job_handler.h, task_queue.h）
├── src/                # 程式原始碼（main.c, job_handler.c, client.c, task_queue.c）
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

## ⚙️ Semaphore 與排程設定

- `cargo_sem`：最多同時 3 個 Cargo 任務（material_delivery, intermediate_transfer）可執行。
- `qc_sem`：最多同時 1 個 QC 任務（sample_collection）可執行。
- Cargo 任務額外使用 Dispatcher Thread 處理優先級佇列，確保高優先級任務先被執行。

### 任務類型對應說明

| 任務名稱               | 說明                         | 所使用 semaphore  |
|-----------------------|------------------------------|------------------|
| material_delivery     | 從倉儲區搬送物料至工作站       | cargo_sem        |
| intermediate_transfer | 在不同製程單元間搬運半成品     | cargo_sem        |
| sample_collection     | 從指定感測點採集樣本          | qc_sem           |

### 任務處理流程

> **說明**：目前任務處理過程中使用 `sleep()` 模擬機器人任務耗時，並非真實動作。所有任務類型會依據其資源對應 semaphore 控制。Cargo 類型任務會依優先級排入 queue，由 Dispatcher Thread 負責依序分派處理。

---

## ✅ 已完成功能
- 任務佇列優先級排程（含 Dispatcher Thread）
- 多任務類型對應 semaphore 控制
- 多 client 模擬測試腳本
- 跨主機部署支援（Server 可接受來自不同實體主機的 client 連線）

---

## 📎 未來可擴充方向

- ✅ **加入任務優先級與排程策略**：Cargo 任務已根據 `priority` 欄位排入優先級佇列處理。
- ✅ **跨機部署支援**：目前 client 程式已可改變 IP，支援不同主機間連線。
- ⬜ **容器化（Docker）**：將 server/client 打包成 container，方便測試與部署。
- ⬜ **前端 UI 控制台**：實作 Web UI，即時監控任務分配、資源使用與 queue 狀態。

---

