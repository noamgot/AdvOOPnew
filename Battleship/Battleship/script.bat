@echo off
mkdir myresults REM create a directory 
set games_path=C:\Users\USER\Source\Repos\AdvOOPnew\Battleship\Battleship\input
for /f "tokens=*" %%a in (tests_names.txt) do (
:: printing the test name - you can delete the next 2 lines if you like
  echo ************************************************
  echo %%a
  :: IMPORTATNT - don't forget to change the exe file name!!!
  battleship.exe %games_path%/%%a -quiet > ./myresults/%%a.txt 
 
)
pause