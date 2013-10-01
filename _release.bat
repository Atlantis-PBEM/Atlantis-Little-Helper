set zipfile=ReleaseFiles\ah_2.6.5b.zip
set zipfile2=ReleaseFiles\ah_2.6.5b.no_python.zip

mkdir ReleaseFiles
rm %zipfile%
rm %zipfile2%

pkzip -a  %zipfile% release\ah.exe
pkzip -a -P %zipfile% doc\history.txt doc\readme.html terrain_bitmaps\*


pkzip -a %zipfile2% release_no_py\ah.exe
pkzip -a -P %zipfile2% doc\history.txt doc\readme.html terrain_bitmaps\*

set zipfile=
set zipfile2=
