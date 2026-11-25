@echo off
cd /d "%~dp0"

where pythonw >nul 2>&1
if %errorlevel%==0 (
    start "" pythonw "%~dp0simulador_visual.py"
    exit /b 0
)

where python >nul 2>&1
if %errorlevel%==0 (
    start "" python "%~dp0simulador_visual.py"
    exit /b 0
)

where py >nul 2>&1
if %errorlevel%==0 (
    start "" pyw "%~dp0simulador_visual.py"
    exit /b 0
)

echo [ERRO] Python 3 nao encontrado no PATH do Windows.
echo Instale o Python 3 em https://www.python.org/downloads/ e tente novamente.
pause
exit /b 1
