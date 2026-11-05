@echo off
rem Build and run both SPHINCS+ baselines for one parameter set with MinGW gcc.
rem Usage: build.bat 128f        (any of 128s 128f 192s 192f 256s 256f)
setlocal
set PS=%1
if "%PS%"=="" set PS=128f
set CFLAGS=-O3 -std=c99 -march=native -fomit-frame-pointer
set SRC=hash_sha2.c hash_sha2x8.c thash_sha2_simple.c thash_sha2_simplex8.c sha2.c sha256x8.c sha512x4.c sha256avx.c address.c merkle.c wots.c utils.c utilsx8.c fors.c sign.c
set ROOT=%~dp0
for %%V in (master-7ec789a fips205-ef36d20) do call :one %%V
goto :eof
:one
set V=%1
set H=bench_fips205.c
if "%V%"=="master-7ec789a" set H=bench_master.c
set OUT=%ROOT%out\%V%\%PS%
if not exist "%OUT%" mkdir "%OUT%"
pushd "%ROOT%sphincsplus-%V%\sha2-avx2"
gcc %CFLAGS% -I. -DPARAMS=sphincs-sha2-%PS% -o "%OUT%\bench.exe" %SRC% "%ROOT%harness\bench_randombytes.c" "%ROOT%harness\%H%" || (popd & exit /b 1)
popd
pushd "%OUT%"
bench.exe
popd
goto :eof
