#!/bin/bash

try(){
  input="$1"

  ./cc "$input"
}

try 11
try 15 + 2
try "(3 + 3) / 2"
echo ok
