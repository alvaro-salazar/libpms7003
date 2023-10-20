#!/bin/sh
# 
# File:   build_run_example.sh
# Author: Alvaro Salazar <alvaro@denkitronik.com>
#
# Created on 29/04/2019, 11:55:52 PM
#
gcc example.c -lwiringPi -lpms7003 -o example
sudo ./example