
cmd /k "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

cmake -S . -B ./build -G "Ninja Multi-Config" #-DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release

cmake --build ./build --parallel 10 --config=Release 

pause