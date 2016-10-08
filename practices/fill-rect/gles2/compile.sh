#! /bin/bash
  # Copyright (C) 2016, Gepard Graphics, Szilard Ledan <szledan@gmail.com>

# Collect the "cpp" files to compile.
cpps=`ls *.cpp`

for cpp in $cpps; do
  echo Compile: $cpp

  # Comiple the sample file.
  g++ -std=c++11 -pedantic -Wall -o $cpp.out  $cpp -lX11 -lEGL -lGLESv2
  error_code=$?

  # Checking compilation result. The (error_code == 0) means build ready.
  if [ $error_code != 0 ] ; then
    exit $error_code
  fi
done
