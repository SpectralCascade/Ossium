"""
MIT License

Copyright (c) 2019-2020 Tim Lane

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""
#!/usr/bin/python3

import sys
import os

extensions = [".h", ".cpp", ".hpp", ".cs"]

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
        try:
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
        except UnicodeDecodeError as error:
            print("Warning: Could not insert copyright! UnicodeDecodeError: ", error)
            return
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
    Walk(walkDir, extensions, InsertCopyright, [startIdent + "\n *  \n" + copyrightNotice + " *  \n" + endIdent + "\n"])
    return 0

if (__name__ == "__main__"):
    main()

