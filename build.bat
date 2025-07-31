python3 preprocessor.py
clang -o spark.exe main.c -lgdi32 -luser32 -lkernel32 -lmsimg32

@echo off
if %ERRORLEVEL% EQU 0 (spark)