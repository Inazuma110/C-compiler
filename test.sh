#!/bin/bash

try(){
  expected="$1"
  input="$2"

  ./cc "$input" > tmp.s
  gcc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$expected expected, but got $actual"
    exit 1
  fi
}

try 42 "42;"
try 21 "5+20-4;"
try 41 " 12 + 34 - 5 ;"
try 47 "5+6* 7;"
try 2 "4 / 2;"
try 15 "5*(9-6);"
try 4 "(3+ 5) /2;"
try 16 "((2 + 1)+ (1 + 2)) * 3 - 8 / 4;"
try 10 "a=10;"
try 100 "a=10;b=10;a*b;"
try 2 "a = b = 2; (a + b) / 2;"

echo ok
