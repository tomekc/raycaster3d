@echo off
cls
echo Daj mi dyskietke BACKUP i nadus klawisz
pause >nul
if not exist tajsry.arj goto spakuj
arj f tajsry
goto kopiowanie
:spakuj
arj a tajsry
:kopiowanie
echo ÿ
echo ÿ
echo   Kopiowanie plikow, moze to potrwac chwilke...
copy tajsry.arj a:\k1
copy tajsry.arj a:\k2
