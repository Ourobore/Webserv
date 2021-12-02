#!/bin/bash
for port in {9000..9049}
do
    #lsof -i
    siege -b -c 1 -r 1 http://localhost:$port/script.js
done