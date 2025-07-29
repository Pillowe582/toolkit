@echo off
timeout 3
xcopy "./Pillowe's Toolkit" "." /s /e /y
rd /s /q "./Pillowe's Toolkit"
start "" "PilloweMain.exe"

