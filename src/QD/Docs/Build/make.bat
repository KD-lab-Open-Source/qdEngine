@echo off
md html
copy background.gif html
doxygen qd_engine.cfg > log.txt
