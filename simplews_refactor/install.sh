#!/usr/bin/env bash
set -e

echo "== Go WebSocket Server Installer =="

# --- Verify Homebrew ---
if ! command -v brew >/dev/null 2>&1; then
  echo "Homebrew is required but not installed."
  exit 1
fi

# --- Install dependencies ---
if ! command -v colima >/dev/null 2>&1; then
  brew install colima
fi

if ! command -v docker >/dev/null 2>&1; then
  brew install docker
fi

# --- Start Colima ---
if ! colima status 2>/dev/null | grep -q Running; then
  colima start --cpu 2 --memory 2 --disk 10
fi

# --- Verify Docker ---
if ! docker info >/dev/null 2>&1; then
  echo "Docker is not available."
  exit 1
fi

# --- Verify Go module ---
if [ ! -f go.mod ]; then
  echo "go.mod missing. Aborting."
  exit 1
fi

if [ ! -f go.sum ]; then
  echo "go.sum missing. Running 'go mod tidy'..."
  go mod tidy
fi

# --- Build and run ---
docker compose build
docker compose up -d

echo "Server running at http://localhost:8080"
