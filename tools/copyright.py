#!/usr/bin/python3

import sys
import os

extensions = [".h", ".cpp", ".hpp"]

startIdent = "/** COPYRIGHT NOTICE"
endIdent = "**/"

def Walk(path, filetypes, filefunc, args):
    for root, dir, files in os.walk(path):
        for name in files:
            for ext in extensions:
                if len(ext) < len(name) and name[-len(ext):] == ext:
                    filefunc(os.path.join(root, name), args)
                    break

def InsertCopyright(path, args):
    if (len(args) < 1):
        print("Error, no copyright text specified!")
        raise
    print(f"Inserting copyright for file: {path}")
    outputStr = ""
    inserting = False
    with open(path) as file:
        newHeader = False
        firstLine = True
        for line in file.readlines():
            if (firstLine and not newHeader and startIdent not in line):
                newHeader = True
                outputStr += args[0]
            else:
                firstLine = False
            if (inserting):
                if (endIdent in line):
                    inserting = False
                    # skip the new line
                    continue
                else:
                    continue
            elif (startIdent in line):
                inserting = True
                outputStr += args[0]
            else:
                outputStr += line
    with open(path, "w+") as file:
        file.write(outputStr)

def main():
    global extensions
    global startIdent
    global endIdent
    walkDir = ""
    copyrightNotice = ""
    if (len(sys.argv) < 3):
        print(f"Error: No copyright file specified! Command format is: python3 copyright.py \"copyrightFile.txt\" \"/walk/directory/path\"")
        return -1
    else:
        walkDir = sys.argv[2]
        with open(sys.argv[1]) as file:
            for line in file.readlines():
                copyrightNotice += " *  " + line
    Walk(walkDir, extensions, InsertCopyright, [startIdent + "\n *  \n" + copyrightNotice + "\n *  \n" + endIdent + "\n"])
    return 0

if (__name__ == "__main__"):
    main()

