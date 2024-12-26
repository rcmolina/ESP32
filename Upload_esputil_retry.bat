@for /f "tokens=4 delims=\: " %%P IN ('mode ^|find "COM"') do esputil -b 115200 -p %%P flash 0x0 %1
pause