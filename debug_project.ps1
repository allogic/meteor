$project = $args[0]

Push-Location $project
	Push-Location build
		lldb "$($project).exe"
	Pop-Location
Pop-Location
