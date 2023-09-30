#!/bin/sh

[ ! -f UtfConv.c ] && curl -O https://www.alphabet.se/download/UtfConv.c
[ ! -f  UtfConv.o ] && clang UtfConv.c -c

utfcpp_include=utfcpp/source

clang -std=c++11 -lc++ -I$utfcpp_include \
    -o demo \
    demo.cpp \
    transcriber.cpp \
    converter.cpp \
    rules.cpp \
    data_rules.cpp \
    data_exceptions.cpp \
    data_charsets.cpp \
    util.cpp \
    utf8lower.cpp \
    UtfConv.o
