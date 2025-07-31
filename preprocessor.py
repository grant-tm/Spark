import os
import re

EXCLUDE_DIR = 'external'

def find_c_files(base_dir):
    
    c_files = []
    
    for root, dirs, files in os.walk(base_dir):
        
        if EXCLUDE_DIR in root.split(os.sep):
            continue
        
        for file in files:
            if file.endswith('.c'):
                c_files.append(os.path.join(root, file))
    
    return c_files

def generate_header_path(c_file_path):
    return c_file_path.rsplit('.', 1)[0] + '.h'

def extract_structs_and_funcs(source):
    
    # Capture struct definition and name
    struct_defs = re.findall(r'(struct\s+([A-Za-z_][A-Za-z_0-9]*)\s*\{[^}]*\};)', source)

    # Capture lines starting with "def" and ending with "{"
    func_defs = re.findall(r'def\s+([a-zA-Z_][\w\s\*\(\),]*?)\s*\{', source)

    return struct_defs, func_defs


def write_header(header_path, structs, funcs):
    
    with open(header_path, 'w') as f:
        f.write('#pragma once\n\n')

        for full_struct, struct_name in structs:
            f.write(f'typedef struct {struct_name} {struct_name};\n')
        
        if structs:
            f.write('\n')

        for full_struct, _ in structs:
            f.write(full_struct + '\n\n')

        for decl in funcs:
            decl_clean = re.sub(r'\s+', ' ', decl.strip())
            f.write(decl_clean + ';\n')

def process_file(c_file_path):
    
    with open(c_file_path, 'r') as f:
        source = f.read()

    structs, funcs = extract_structs_and_funcs(source)
    if not structs and not funcs:
        return

    h_file_path = generate_header_path(c_file_path)
    write_header(h_file_path, structs, funcs)
    print(f'Generated {h_file_path}')

def main():
    base_dir = os.getcwd()
    c_files = find_c_files(base_dir)
    for c_file in c_files:
        process_file(c_file)

if __name__ == '__main__':
    main()
