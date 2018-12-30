#!/bin/bash

try(){
  input="$1"

  ./cc "$input"
}

try "(3 + 3) / 2;"
try "a=10;"
echo ok
