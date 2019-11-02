#!/usr/bin/python3
import ctypes
import time
import pymysql
 
lib = ctypes.CDLL("libc.so.6")

con_content = pymysql.connect("127.0.0.1", "dev_leaf", "dev_pass", "lxhblogspace_content", port=33060)
while True:
	cursor = con_content.cursor()
	cursor.execute("SELECT article_id, 
