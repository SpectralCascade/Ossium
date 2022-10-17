#!python3

import sys
import os
import time
import shutil

start_time = 0

# Log a message to the console
def log(message):
    log_always(message)

# Always log no matter what
def log_always(message):
    print(("[{:.3f}] ".format(time.time() - start_time)) + message)

# Execute a shell command
def execute(command, ps = False):
    log("$ " + command)
    return os.system(("powershell.exe " if ps else "") + command)

def gendir(dest, variant, output=""):
    return os.path.normpath(os.path.join(dest, variant, output))

# Standard execution
# py buildshaders.py -i shaders_path -o output_path [-p platform] [-c compiler_path]
if __name__ == "__main__":
    # Initial variables setup
    start_time = time.time()
    cwd = os.getcwd()
    script_path = os.path.abspath(sys.argv[0])
    argv = sys.argv
    argc = len(argv)
    platform = "windows"
    compiler = "shaderc.exe"
    src = ""
    dest = ""
    
    # Get arguments
    i = 1
    success = 0
    while (i < argc):
        if (argv[i] == "-p" and i + 1 < argc):
            i += 1
            platform = argv[i]
        if (argv[i] == "-c" and i + 1 < argc):
            i += 1
            compiler = os.path.normpath(argv[i])
        if (argv[i] == "-i" and i + 1 < argc):
            i += 1
            src = argv[i]
            success = success | 1
        if (argv[i] == "-o" and i + 1 < argc):
            i += 1
            dest = argv[i]
            success = success | 2
        i += 1
        
    if (not (success & 3)):
        log_always("Invalid arguments! You must always specify an input and output path.")
        log_always("Options:\n-i shaders_path \t: Specify a path to the source shaders.\n-o output_path \t: Specify a path to output the compiled shaders.\n[-p platform] \t: Optionally specify a specific platform. This defaults to windows otherwise.\n[-c compiler_path] \t : Optionally specify path to the shader compiler executable.")
        sys.exit(-1)
    
    src = os.path.normpath(src)
    dest = os.path.normpath(dest)
    dir = os.path.normpath(os.path.join(cwd, src))
    type = "v"
    for shader in os.listdir(dir):
        file = os.fsdecode(shader)
        input = os.path.normpath(os.path.join(dir, file))
        output = file + ".bin"
        command = "\"" + compiler + "\" -i \"" + "C:\\Users\\2013b\\AppData\\Local\\Soupbuild\\source\\bgfx-2022.09.12\\src" + "\" -f \"" + input + "\" --platform " + platform + " --type "
        
        if file.endswith(".vert"):
            type = "v"
        elif file.endswith(".frag"):
            type = "f"
        elif file.endswith(".comp"):
            type = "c"
        else:
            continue
        command += type
        if (type == "f"):
            type = "p"
        execute("mkdir \"" + dest + "\" > nul")
        command += " -o \""
        
        # TODO support other platforms
        match platform:
            case "windows" | _:
                execute("mkdir \"" + gendir(dest, "glsl") + "\" > nul")
                execute("mkdir \"" + gendir(dest, "dx9") + "\" > nul")
                execute("mkdir \"" + gendir(dest, "dx11") + "\" > nul")
                execute("mkdir \"" + gendir(dest, "spirv") + "\" > nul")
                execute(command + gendir(dest, "glsl", output) + "\"", True)
                execute(command + gendir(dest, "dx9", output) + "\" -p " + type + "s_5_0 ", True)
                execute(command + gendir(dest, "dx11", output) + "\" -p " + type + "s_5_0 ", True)
                execute(command + gendir(dest, "spirv", output) + "\" -p spirv ", True)
        
        
    
    
