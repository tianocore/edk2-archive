##**********************************************************************
## Function define
##**********************************************************************
function Usage() {
  echo
  echo "Usage: BuildAll.bat [/ia32 | /x64]  "
  echo
  echo "If NO flags, both are built.        "
  echo
  echo "       /ia32  Build only 32 bit     "
  echo "       /x64   Build only 64 bit     "
  echo
  echo "Press any key......                 "
  read
  exit 0
}

##
## This script will attempt all the critical build combinations
## (it will not build all combination, as there are way to many)
##

##
## If you think a need build combination is missing, please add it below
##

echo
if [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/x64" ]; then
  ./BuildIFWI.sh /q /vp /x64 APLK debug
  ./BuildIFWI.sh /q /vp /x64 BXTM debug
  ./BuildIFWI.sh /q /vp /x64 /csle BXTM debug
  ./BuildIFWI.sh /q /vp /x64 /sv BXTM debug
  ./BuildIFWI.sh /q /vp /x64 APLK release
  ./BuildIFWI.sh /q /vp /x64 BXTM release

else
  ./BuildIFWI.sh /q /vp APLK debug
  ./BuildIFWI.sh /q /vp BXTM debug
  ./BuildIFWI.sh /q /vp APLK release
  ./BuildIFWI.sh /q /vp BXTM release
fi