cd %1
del SkinRes.zip
cd SkinRes
..\7za.exe a ..\SkinRes.zip -x!.svn
cd ..
exit