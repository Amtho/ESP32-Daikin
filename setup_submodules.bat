@echo off
REM Fetch all submodules recursively
if not defined GIT (
  where git >nul 2>nul || (
    echo Git not found. Please install Git and retry.
    goto end
  )
)

git submodule update --init --recursive

:end
