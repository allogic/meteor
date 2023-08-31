import os
import sys
import shutil
import platform

def configure_llvm():
	llvm_path=f'llvm/llvm'
	build_path=f'llvm/build'
	if platform.system() == 'Windows':
		install_path=f'C:/llvm'
	elif platform.system() == 'Linux':
		install_path=f'/opt/llvm'
	if os.path.exists(build_path):
		shutil.rmtree(build_path)
	os.mkdir(build_path)
	os.system(f'cmake -D "LLVM_ENABLE_PROJECTS=clang;lld;lldb" -D "CMAKE_BUILD_TYPE=Release" -D "CMAKE_INSTALL_PREFIX={install_path}" -G "Unix Makefiles" -S "{llvm_path}" -B "{build_path}"')

def build_llvm():
	build_path=f'llvm/build'
	os.system(f'cmake --build "{build_path}" --config Release --parallel 8')

def install_llvm():
	build_path=f'llvm/build'
	os.system(f'cmake --install "{build_path}"')

cmd=sys.argv[1]

if 'c' in cmd:
	configure_llvm()

if 'b' in cmd:
	build_llvm()

if 'i' in cmd:
	install_llvm()
