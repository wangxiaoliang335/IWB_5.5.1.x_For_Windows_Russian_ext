for /F %%F in ('dir /B *.pdb') do (
 rd /S /Q %%F
)