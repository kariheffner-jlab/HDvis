import sys
import os
 
jana_home = os.environ['JANA_HOME']
jana_include = os.path.join(jana_home, 'include')
print(jana_include)
# SConstruct
env = Environment()
env.Append(CPPPATH = [os.path.join(jana_home,'include')])
hello = Program(["main.cpp"])
