#!/bin/bash
# =============================================================================
# Rate Limiter — Concurrency Benchmark Script
# =============================================================================
# Usage: ./benchmarks/run_benchmark.sh [algorithm]
# Prerequisites: Docker running, `wrk` or `ab` installed
#
# This script:
#   1. Spins up the Docker cluster with the chosen algorithm
#   2. Waits for the API to be healthy
#   3. Runs a wrk/ab load test
#   4. Captures throughput, latency, and error metrics
#   5. Tears down the cluster

set -euo pipefail

ALGORITHM="${1:-redis_fixed_window}"
DURATION="10s"
THREADS=8
CONNECTIONS=100
URL="http://localhost:8080/api/v1/rate-limit"

echo "============================================="
echo " Rate Limiter Benchmark"
echo " Algorithm:   $ALGORITHM"
echo " Duration:    $DURATION"
echo " Threads:     $THREADS"
echo " Connections: $CONNECTIONS"
echo "============================================="

# --- 1. Start cluster ---
echo "[1/4] Starting Docker cluster with ALGORITHM=$ALGORITHM..."
ALGORITHM=$ALGORITHM docker compose up -d --build

# --- 2. Wait for API health ---
echo "[2/4] Waiting for API to be ready..."
for i in $(seq 1 30); do
    if curl -s -o /dev/null -w "%{http_code}" -X POST \
        -H "Content-Type: application/json" \
        -d '{"clientId":"health_check"}' \
        "$URL" 2>/dev/null | grep -q "200\|429"; then
        echo "       API is ready! (attempt $i)"
        break
    fi
    sleep 1
    if [ "$i" -eq 30 ]; then
        echo "ERROR: API failed to start within 30 seconds"
        docker compose down
        exit 1
    fi
done

# --- 3. Create wrk Lua script for POST requests ---
WRK_SCRIPT=$(mktemp /tmp/wrk_post_XXXXXX.lua)
cat > "$WRK_SCRIPT" << 'EOF'
-- wrk Lua script: sends JSON POST with rotating client IDs
-- This simulates realistic multi-tenant traffic
counter = 0

wrk.method = "POST"
wrk.headers["Content-Type"] = "application/json"

request = function()
    counter = counter + 1
    local client_id = "client_" .. (counter % 100)
    wrk.body = '{"clientId":"' .. client_id .. '"}'
    return wrk.format(nil, "/api/v1/rate-limit")
end
EOF

# --- 4. Run benchmark ---
echo "[3/4] Running load test..."
echo ""

# Check if wrk is available, fall back to ab
if command -v wrk &>/dev/null; then
    echo "Using wrk (HTTP benchmarking tool)"
    echo "---------------------------------------------"
    wrk -t"$THREADS" -c"$CONNECTIONS" -d"$DURATION" -s "$WRK_SCRIPT" "$URL"
    echo ""
elif command -v ab &>/dev/null; then
    echo "Using Apache Bench (ab)"
    echo "---------------------------------------------"
    # ab doesn't support Lua, so we use a fixed client ID
    echo '{"clientId":"benchmark_client"}' > /tmp/ab_post_data.json
    ab -n 10000 -c "$CONNECTIONS" -T "application/json" \
       -p /tmp/ab_post_data.json \
       "$URL"
    rm -f /tmp/ab_post_data.json
else
    echo "ERROR: Neither 'wrk' nor 'ab' found."
    echo "Install with:"
    echo "  Ubuntu:  sudo apt install wrk apache2-utils"
    echo "  macOS:   brew install wrk"
    docker compose down
    exit 1
fi

# Cleanup temp script
rm -f "$WRK_SCRIPT"

# --- 5. Tear down ---
echo "[4/4] Tearing down Docker cluster..."
docker compose down

echo ""
echo "============================================="
echo " Benchmark complete!"
echo "============================================="
