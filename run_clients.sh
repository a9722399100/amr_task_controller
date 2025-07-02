#!/bin/bash

# 七個不同任務
task1="REQUEST job=material_delivery; from=storage_A; to=station_1"
task2="REQUEST job=material_delivery; from=storage_B; to=station_2"
task3="REQUEST job=intermediate_transfer; from=cell_X; to=station_Y"
task4="REQUEST job=intermediate_transfer; from=cell_Y; to=station_Z"
task5="REQUEST job=sample_collection; source=qc_node_1"
task6="REQUEST job=sample_collection; source=qc_node_2"
task7="REQUEST job=sample_collection; source=qc_node_3"

echo "開始發送不同任務至伺服器..."

./build/client "$task1" &
./build/client "$task2" &
./build/client "$task3" &
./build/client "$task4" &
./build/client "$task5" &
./build/client "$task6" &
./build/client "$task7" &

wait
echo "[✓] 所有任務已執行完畢"

