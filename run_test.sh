#!/bin/bash

set -e
io/make test1
io/make test1

echo $'running 1st test\n'
apps/test1

echo$'running 2nd test\n'
apps/test2