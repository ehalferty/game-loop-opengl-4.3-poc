#!/bin/bash

javac Game.java
# This header isn't actually used, I just regenerate it for reference.
javah Game

cp Game.class ./cmake*/
