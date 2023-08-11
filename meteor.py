import os
import pty
import sys
import shutil
import subprocess
import platform

def exec_command(command):
	try:
		master,slave=pty.openpty()
		process=subprocess.Popen(command, shell=True, cwd='.', stdin=subprocess.PIPE, stdout=slave, stderr=slave, close_fds=True)
		process.communicate()
		os.close(slave)
		while True:
			output=os.read(master, 1024)
			if not output:
				break
			print(output.decode('utf-8'), end='')
	except Exception as e:
		print('')

def configure(project, configuration):
	project_path=f'projects/{project}'
	build_path=f'projects/{project}/build'
	if os.path.exists(build_path):
		shutil.rmtree(build_path)
	os.mkdir(build_path)
	exec_command(f'cmake -D"CMAKE_C_COMPILER=clang" -D"CMAKE_BUILD_TYPE={configuration}" -G "Unix Makefiles" -S"{project_path}" -B"{build_path}"')

def build(project):
	exec_command(f'cmake --build "projects/{project}/build" --parallel 8')

def run(project):
	if platform.system() == 'Windows':
		exec_command(f'projects/{project}/build/{project}.exe')
	if platform.system() == 'Linux':
		exec_command(f'projects/{project}/build/{project}')

def shader(project):
	shader_path=f'projects/{project}/shaders'
	if os.path.exists(shader_path):
		for file_name in os.listdir(shader_path):
			file_stem, file_ext = os.path.splitext(file_name)
			file_path = os.path.join(shader_path, file_stem)
			if os.path.isfile(f'{file_path}.glsl'):
				exec_command(f'glslc {file_path}.glsl -o {file_path}.spv')

cmd=sys.argv[1]

if (cmd==f'c'):
	project = sys.argv[2]
	configuration = sys.argv[3]
	configure(project, configuration)

if (cmd==f'b'):
	project = sys.argv[2]
	build(project)

if (cmd==f'r'):
	project = sys.argv[2]
	run(project)

if (cmd==f's'):
	project = sys.argv[2]
	shader(project)

if (cmd==f'cb'):
	project = sys.argv[2]
	configuration = sys.argv[3]
	configure(project, configuration)
	build(project)

if (cmd==f'br'):
	project = sys.argv[2]
	build(project)
	run(project)

if (cmd==f'cbr'):
	project = sys.argv[2]
	configuration = sys.argv[3]
	configure(project, configuration)
	build(project)
	run(project)
