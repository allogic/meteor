import os
import sys
import shutil
import platform

if platform.system() == 'Windows':
	os.environ['PATH'] += os.pathsep + 'C:/llvm/bin'
elif platform.system() == 'Linux':
	os.environ['PATH'] += os.pathsep + '/opt/llvm/bin'

def llvm():
	llvm_path=f'llvm/llvm'
	build_path=f'llvm/build'
	if platform.system() == 'Windows':
		install_path=f'C:/llvm'
	elif platform.system() == 'Linux':
		install_path=f'/opt/llvm'
	if os.path.exists(build_path):
		shutil.rmtree(build_path)
	os.mkdir(build_path)
	if os.path.exists(install_path):
		shutil.rmtree(install_path)
	os.mkdir(install_path)
	os.system(f'cmake -D"LLVM_ENABLE_PROJECTS=clang;lld;lldb" -D"CMAKE_BUILD_TYPE=Release" -D"CMAKE_INSTALL_PREFIX={install_path}" -G "Unix Makefiles" -S"{llvm_path}" -B"{build_path}"')
	os.system(f'cmake --build "{build_path}" --parallel 8')
	os.system(f'cmake --install "{build_path}"')

def configure(project, configuration):
	project_path=f'projects/{project}'
	build_path=f'projects/{project}/build'
	if os.path.exists(build_path):
		shutil.rmtree(build_path)
	os.mkdir(build_path)
	os.system(f'cmake -D"CMAKE_C_COMPILER=clang" -D"CMAKE_BUILD_TYPE={configuration}" -G "Unix Makefiles" -S"{project_path}" -B"{build_path}"')

def build(project):
	os.system(f'cmake --build "projects/{project}/build" --parallel 8')

def run(project):
	build_path=f'projects/{project}/build'
	os.chdir(build_path)
	os.system(f'{project}')

def debug(project):
	build_path=f'projects/{project}/build'
	os.chdir(build_path)
	os.system(f'lldb {project}')

def shader(project):
	shader_path=f'projects/{project}/shaders'
	if os.path.exists(shader_path):
		for file_name in os.listdir(shader_path):
			file_stem,file_ext=os.path.splitext(file_name)
			file_path=f'{shader_path}/{file_stem}'
			if file_ext=='.vert' or file_ext=='.frag' or file_ext=='.comp':
				os.system(f'glslc {file_path}{file_ext} -o {file_path}{file_ext}.spv')

cmd=sys.argv[1]

if cmd=='llvm':
	llvm()

if cmd==f'c' or cmd=='configure':
	project = sys.argv[2]
	configuration = sys.argv[3]
	configure(project, configuration)

if cmd==f'b' or cmd=='build':
	project = sys.argv[2]
	build(project)

if cmd==f'r' or cmd=='run':
	project = sys.argv[2]
	run(project)

if cmd==f'd' or cmd=='debug':
	project = sys.argv[2]
	debug(project)

if cmd==f's' or cmd=='shader':
	project = sys.argv[2]
	shader(project)

if cmd==f'cb':
	project = sys.argv[2]
	configuration = sys.argv[3]
	configure(project, configuration)
	build(project)

if cmd==f'br':
	project = sys.argv[2]
	build(project)
	run(project)

if cmd==f'bd':
	project = sys.argv[2]
	build(project)
	debug(project)

if cmd==f'cbr':
	project = sys.argv[2]
	configuration = sys.argv[3]
	configure(project, configuration)
	build(project)
	run(project)

if cmd==f'cbd':
	project = sys.argv[2]
	configuration = sys.argv[3]
	configure(project, configuration)
	build(project)
	debug(project)