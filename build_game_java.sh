#!/bin/bash

javac Game.java
# This header isn't actually used, I just regenerate it for reference.
javac -h . Game.java
# javah Game

cp Game.class ./cmake*/
