#!/bin/sh

gcc -DDEBUG -o dict -fms-extensions -Wno-microsoft-anon-tag dict.c test.c
