#!/bin/bash

export BOOST_ROOT=$PWD/deps/root-clang
export FMT_ROOT=$PWD/deps/fmt

export CXX=clang++

export BUILD_DIR=$PWD/build/validator/build
export INSTALL_DIR=$PWD/build/validator/install
export SRC_DIR=$PWD/cpp-validator

if [ -d "$BUILD_DIR" ]; then
  rm -rf $BUILD_DIR
fi

if [ -d "$INSTALL_DIR" ]; then
  rm -rf $INSTALL_DIR
fi

mkdir -p $BUILD_DIR
cd $BUILD_DIR

cmake -G "CodeBlocks - Unix Makefiles" -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR -DCMAKE_BUILD_TYPE=Release -DVALIDATOR_WITH_TESTS=On -DVALIDATOR_WITH_FMT=On $SRC_DIR
make -j8 install
cd -

$BUILD_DIR/test/dracoshavalidator-test --log_level=test_suite
