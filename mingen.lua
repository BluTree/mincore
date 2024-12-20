mg.configurations({"debug", "release"})

local mincore = mg.project({
	name = "mincore",
	type = mg.project_type.sources,
	sources = {"src/**.cc"},
	compile_options = {"-g", "-Wall", "-Wextra", "-nostdinc++", "--std=c++20"},
	debug = {
		compile_options = {"-O0", "-fprofile-instr-generate", "-fcoverage-mapping"}
	},
	release = {
		compile_options = {"-O2", "-Werror"}
	},
})

linker_type_arg = ""
if mg.platform() == "windows" then
	linker_type_arg = "-fuse-ld=lld"
elseif mg.platform() == "linux" then
	linker_type_arg = "-fuse-ld=ld"
end

local tests_exe = mg.project({
	name = "tests",
	type = mg.project_type.executable,
	sources = {"tests/**.cc"},
	includes = {"src"},
	compile_options = {"-g", "-O0", "-Wall", "-Wextra", "-Werror", "-fno-char8_t", "-nostdinc++", "--std=c++20", "-D_CRT_SECURE_NO_WARNINGS", "-fprofile-instr-generate", "-fcoverage-mapping"},
	link_options = {linker_type_arg, "-g", "-fprofile-instr-generate", "-fcoverage-mapping"},
	dependencies = {mincore}
})

mg.generate({tests_exe})