A small convertor I built for myself. 

Usage: /home/voyager-1/scripts/convertor [ -atb | -bta ] YYYY MM DD
  -bta: Convert input date (BS) to AD (Gregorian)
  -atb: Convert input date (AD) to BS (Nepali)

  To get today's date.
  For unix like systems: 
  convertor -atb $(date "+%Y %m %d")
  for windows systems: 
  convertor.exe -atb (Get-Date -Format "yyyy MM dd")
  To change the format just change it from the source code.
  
  To compile on Windows: 
  gcc convertor.c -o convertor.exe -03 -march=native
  To compile on unix like systems
  gcc convertor.c -o convertor -03 -march=native
