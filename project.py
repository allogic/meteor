import os
import sys
import shutil
import platform

def configure_project(project, configuration):
	project_path=f'projects/{project}'
	build_path=f'projects/{project}/build'
	if os.path.exists(build_path):
		shutil.rmtree(build_path)
	os.mkdir(build_path)
	os.system(f'cmake -D "CMAKE_C_COMPILER=clang" -D "CMAKE_BUILD_TYPE={configuration}" -G "Unix Makefiles" -S "{project_path}" -B "{build_path}"')

def build_project(project, configuration):
	build_path=f'projects/{project}/build'
	os.system(f'cmake --build "{build_path}" --config {configuration} --parallel 8')

def run_project(project):
	root_path=os.getcwd()
	project_path=f'projects/{project}'
	if platform.system() == 'Windows':
		src_path=f'projects/{project}/build/{project}.exe'
		dst_path=f'projects/{project}/{project}.exe'
	elif platform.system() == 'Linux':
		src_path=f'projects/{project}/build/{project}'
		dst_path=f'projects/{project}/{project}'
	if os.path.exists(dst_path):
		os.remove(dst_path)
	shutil.copy(src_path, dst_path)
	os.chdir(project_path)
	if platform.system() == 'Windows':
		os.system(f'{project}')
	elif platform.system() == 'Linux':
		os.system(f'./{project}')
	os.chdir(root_path)
	os.remove(dst_path)

def debug_project(project):
	root_path=os.getcwd()
	project_path=f'projects/{project}'
	if platform.system() == 'Windows':
		src_path=f'projects/{project}/build/{project}.exe'
		dst_path=f'projects/{project}/{project}.exe'
	elif platform.system() == 'Linux':
		src_path=f'projects/{project}/build/{project}'
		dst_path=f'projects/{project}/{project}'
	if os.path.exists(dst_path):
		os.remove(dst_path)
	shutil.copy(src_path, dst_path)
	os.chdir(project_path)
	os.system(f'lldb {project}')
	os.chdir(root_path)
	os.remove(dst_path)

def build_shader(project):
	shader_path=f'projects/{project}/shaders'
	if os.path.exists(shader_path):
		for file_name in os.listdir(shader_path):
			file_stem,file_ext=os.path.splitext(file_name)
			file_path=f'{shader_path}/{file_stem}'
			if file_ext=='.vert' or file_ext=='.frag' or file_ext=='.comp':
				os.system(f'glslc {file_path}{file_ext} -o {file_path}{file_ext}.spv')

cmd=sys.argv[1]

if 'c' in cmd:
	project = sys.argv[2]
	configuration = sys.argv[3]
	configure_project(project, configuration)

if 'b' in cmd:
	project = sys.argv[2]
	configuration = sys.argv[3]
	build_project(project, configuration)

if 's' in cmd:
	project = sys.argv[2]
	build_shader(project)

if 'r' in cmd:
	project = sys.argv[2]
	run_project(project)

if 'd' in cmd:
	project = sys.argv[2]
	debug_project(project)
