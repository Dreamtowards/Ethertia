## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions
## are met:
##  * Redistributions of source code must retain the above copyright
##    notice, this list of conditions and the following disclaimer.
##  * Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimer in the
##    documentation and/or other materials provided with the distribution.
##  * Neither the name of NVIDIA CORPORATION nor the names of its
##    contributors may be used to endorse or promote products derived
##    from this software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
## EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
## PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
## CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
## EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
## PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
## PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
## OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
## (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
## OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
## Copyright (c) 2008-2023 NVIDIA Corporation. All rights reserved.

# general utility module

import os
import sys
import re
import subprocess

def list_autogen_files(dirPath):
	autogenFiles = []
	for (root, subdirs, files) in os.walk(dirPath):
		files = [f for f in files if re.search(r"AutoGenerated", f)]
		autogenFiles.extend([os.path.join(root, f) for f in files])
	return autogenFiles

#checkout files with p4 if available
def try_checkout_files(files):
	
	print("checking p4 connection parameter...")
	
	# checking p4
	cmd = "p4"
	(stdout, stderr) = run_cmd(cmd)
	
	if stderr == "":
		print("p4 available.")
	else:
		print("p4 unavailable.")
		return
	
	cmd = "p4 edit " + " " + " ".join(files)
	(stdout, stderr) = run_cmd(cmd)
	print(stderr)
	print(stdout)

# check files writability
def check_files_writable(files):
	for file in files:
		if not os.access(file, os.W_OK):
			return False
	return True

# find a root directory containing a known directory (as a hint)
def find_root_path(startDir, containedDir):
	
	currentDir = startDir
	
	# search directory tree
	mergedDir = os.path.join(currentDir, containedDir)
	while not os.path.isdir(mergedDir):
		(currentDir, dir) = os.path.split(currentDir)
		if not dir:
			return None
			
		mergedDir = os.path.join(currentDir, containedDir)
	
	return currentDir
	
def run_cmd(cmd, stdin = ""):
	process = subprocess.Popen(cmd, shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	
	(stdoutRaw, stderrRaw) = process.communicate(stdin.encode('utf-8'))
	stdout = stdoutRaw.decode(encoding='utf-8')
	stderr = stderrRaw.decode(encoding='utf-8')
	return (stdout, stderr)
	
# clears content of files
def clear_files(files):
	for file in files:
		open(file, 'w').close()

##############################################################################
# internal functions
##############################################################################
	

