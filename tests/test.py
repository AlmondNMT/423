import os

for root, dirs, files in os.walk('.'):
    for file in files:
        if file.endswith('.py'):
            filepath = os.path.join(root, file)
            print(filepath)
            os.system(f'./puny {filepath}')
