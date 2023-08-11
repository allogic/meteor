$project = $args[0]
$shader = $args[1]

Push-Location "projects"
	Push-Location $project
		Push-Location "shaders"
			glslc test.vert -o vert.spv
			glslc test.frag -o frag.spv
		Pop-Location
	Pop-Location
Pop-Location
