import os
import re

lineCount = 0

for dirPath, dirNames, fileNames in os.walk('.'):
    for f in fileNames:
        m = re.search('\.cpp|\.h|\.qml', f)
        if m:
            filePath = '{}/{}'.format(dirPath, f)
            with open(filePath) as f:
                lines = f.readlines()
                lineCount += len(lines)

print('Line Count: {}\nVersion: {:.2f}'.format(lineCount, lineCount / 5000))