#!/bin/bash

# job_desc + priority 拆開為兩個參數傳入
echo "開始發送不同任務至伺服器..."

./build/client "REQUEST job=material_delivery; from=storage_A; to=station_1" 2 &
./build/client "REQUEST job=material_delivery; from=storage_B; to=station_2" 3 &
./build/client "REQUEST job=intermediate_transfer; from=cell_X; to=station_Y" 1 &
./build/client "REQUEST job=intermediate_transfer; from=cell_Y; to=station_Z" 2 &
./build/client "REQUEST job=sample_collection; source=qc_node_1" 1 &
./build/client "REQUEST job=sample_collection; source=qc_node_2" 2 &
./build/client "REQUEST job=sample_collection; source=qc_node_3" 3 &

wait
echo "[✓] 所有任務已執行完畢"

