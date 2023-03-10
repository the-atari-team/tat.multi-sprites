#!/bin/bash

# This is a simple download helper script
# to get
# xldir.jar
# wnf-compiler.jar

set -e

cd $(dirname $0)

PWD=$(pwd)

echo "Get everything in here"
CURL_OPTS="-k"
mkdir -p compiler/bin

# check if we found xldir
if [[ -z "$(which xldir)" ]]; then
  echo "No xldir found"
  curl $CURL_OPTS -L -o compiler/bin/xldir.jar "https://github.com/the-atari-team/lla.xldir.disktool/releases/download/latest/xldir.jar"

  cat >compiler/bin/xldir <<__EOF__
#!/bin/bash
java -jar $PWD/compiler/bin/xldir.jar \$@
__EOF__
  chmod +x compiler/bin/xldir
else
  echo "xldir found"
fi


## check if we found xl-packer
#if [[ -z "$(which xl-packer)" ]]; then
#  echo "No xl-packer found"
#  curl $CURL_OPTS -L -o compiler/bin/xl-packer.jar "https://github.com/the-atari-team/tat.packer/releases/download/latest/xl-packer.jar"
#
#  cat >compiler/bin/xl-packer <<__EOF__
##!/bin/bash
#java -jar $PWD/compiler/bin/xl-packer.jar \$@
#__EOF__
#  chmod +x compiler/bin/xl-packer
#else
#  echo "xl-packer found"
#fi


# check if we found wnfc
if [[ -z "$(which wnfc)" ]]; then
  echo "No wnfc found"
  curl $CURL_OPTS -L -o compiler/bin/wnf-compiler.jar "https://github.com/the-atari-team/wnf.compiler/releases/download/latest/wnf-compiler.jar"

  cat >compiler/bin/wnfc <<__EOF__
#!/bin/bash
java -jar $PWD/compiler/bin/wnf-compiler.jar \$@
__EOF__
  chmod +x compiler/bin/wnfc
else
  echo "wnfc found"
fi

if [[ -z "$(which atari800)" ]]; then
  echo "Please install the atari800"
else
  echo "atari800 found"
fi

if [[ -z "$(which atasm)" ]]; then
  echo "Please install the atasm"
else
  echo "atasm found"
fi

mkdir -p firmware

if [[ ! -e firmware/ATARIXL.ROM ]]; then
  echo "Please copy the ATARIXL.ROM firmware to fireware directory"
else
  echo "ATARIXL.ROM firmware found"
fi

if [[ "$PATH" =~ .*compiler/bin.* ]]; then
  echo "found in \$PATH the compiler/bin directory!"
else
  echo "add this to your PATH env-variable"
  echo "export PATH=\$PATH:\$(pwd)/compiler/bin"
fi

echo "if there are no problems, simple call 'make'"
