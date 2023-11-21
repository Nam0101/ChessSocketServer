#!/bin/bash

# Lưu ý: Chắc chắn rằng tệp client.js có quyền thực thi (chmod +x client.js) trước khi chạy kịch bản này.

# Số lần chạy
num_runs=100

# Lệnh chạy
command="node client.js"

# Sử dụng seq để tạo ra các số từ 1 đến 100 và chạy chúng song song bằng xargs
seq $num_runs | xargs -I {} -n 1 -P $num_runs $command
