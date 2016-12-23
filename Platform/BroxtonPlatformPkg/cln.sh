echo
echo Run build cleanall...
echo

echo
echo Cleaning Directories...
echo

if [ -d "Build" ]; then
  rm -r Build
fi

if [ -d "Conf/.cache" ]; then
  rm -r Conf/.cache
fi

echo
echo Cleaning remaining files...
echo

if [ -e $(pwd)/EDK2.log ]; then
  rm $(pwd)/EDK2.log
fi

if [ -e $(pwd)/Unitool.log ]; then
  rm $(pwd)/Unitool.log
fi

if [ -e $(pwd)/Conf/target.txt ]; then
  rm $(pwd)/Conf/target.txt
fi

if [ -e $(pwd)/Conf/BiosId.env ]; then
  rm $(pwd)/Conf/BiosId.env
fi

if [ -e $(pwd)/Conf/tools_def.txt ]; then
  rm $(pwd)/Conf/tools_def.txt
fi

if [ -e $(pwd)/Conf/build_rule.txt ]; then
  rm $(pwd)/Conf/build_rule.txt
fi

if [ -e $(pwd)/Conf/BuildEnv.sh ]; then
  rm $(pwd)/Conf/BuildEnv.sh
fi

if [ -e $(pwd)/Platform/BxtPlatformPkg/DefineAtBuildMacros.dsc ]; then
  rm $(pwd)/Platform/BxtPlatformPkg/DefineAtBuildMacros.dsc
fi

if [ -e $(pwd)/Platform/AplPlatSamplePkg/DefineAtBuildMacros.dsc ]; then
  rm $(pwd)/Platform/AplPlatSamplePkg/DefineAtBuildMacros.dsc
fi

if [ -e $(pwd)/FlashMap.h ]; then
  rm $(pwd)/FlashMap.h
fi

echo
echo All done...
echo