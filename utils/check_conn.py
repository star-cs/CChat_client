import os
import re

project_dir = r"E:\Cpp_Projects\CChat_client\cchat"

connect_pattern = re.compile(r'connect\s*\(([^;]+)\);')

signal_defs = {}
slot_defs = {}

def parse_function_params(param_str):
    param_str = param_str.strip()
    if not param_str:
        return []
    return [p.strip() for p in param_str.split(',')]

def scan_signals_and_slots(file_path):
    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
        lines = f.readlines()
        current_class = None
        in_signals = False
        in_slots = False
        for line in lines:
            class_match = re.search(r'class\s+([a-zA-Z0-9_]+)\s*[:{]', line)
            if class_match:
                current_class = class_match.group(1)
                if current_class not in signal_defs:
                    signal_defs[current_class] = {}
                    slot_defs[current_class] = {}
                in_signals = False
                in_slots = False
                continue

            if 'signals:' in line or 'Q_SIGNALS:' in line:
                in_signals = True
                in_slots = False
                continue
            if 'public slots:' in line or 'private slots:' in line:
                in_signals = False
                in_slots = True
                continue
            if re.match(r'\s*(public|private|protected):', line):
                in_signals = False
                in_slots = False
                continue

            func_match = re.search(r'void\s+([a-zA-Z0-9_]+)\s*\(([^)]*)\)', line)
            if func_match and current_class:
                name = func_match.group(1)
                params = parse_function_params(func_match.group(2))
                if in_signals:
                    signal_defs[current_class][name] = params
                elif in_slots:
                    slot_defs[current_class][name] = params

def scan_connect_calls(file_path):
    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
        lines = f.readlines()
        for i, line in enumerate(lines):
            connect_match = connect_pattern.search(line)
            if connect_match:
                connect_content = connect_match.group(1)
                parts = [p.strip() for p in connect_content.split(',')]
                if len(parts) < 4:
                    continue

                signal_match = re.search(r'&([a-zA-Z0-9_:]+)::([a-zA-Z0-9_]+)', parts[1])
                target_match = re.search(r'&([a-zA-Z0-9_:]+)::([a-zA-Z0-9_]+)', parts[3])

                if signal_match and target_match:
                    signal_class, signal_name = signal_match.groups()
                    target_class, target_name = target_match.groups()

                    signal_params = signal_defs.get(signal_class, {}).get(signal_name, None)
                    target_signal_params = signal_defs.get(target_class, {}).get(target_name, None)
                    target_slot_params   = slot_defs.get(target_class, {}).get(target_name, None)

                    if signal_params is None:
                        print(f"[警告] {file_path}:{i+1} 信号未定义: {signal_class}::{signal_name}")
                        continue

                    # 判断目标是信号还是槽
                    if target_signal_params is not None:
                        target_params = target_signal_params
                        target_type = "信号"
                    elif target_slot_params is not None:
                        target_params = target_slot_params
                        target_type = "槽"
                    else:
                        print(f"[警告] {file_path}:{i+1} 目标未定义: {target_class}::{target_name}")
                        continue

                    # 核心参数匹配检查
                    if len(signal_params) < len(target_params):
                        print(f"[错误] {file_path}:{i+1} {target_type}参数数量超过信号：")
                        print(f"  信号: {signal_class}::{signal_name}({', '.join(signal_params)})")
                        print(f"  {target_type}: {target_class}::{target_name}({', '.join(target_params)})")
                        print(f"  connect: {line.strip()}\n")
                    else:
                        match = True
                        for idx, p in enumerate(target_params):
                            if p != signal_params[idx]:
                                match = False
                                break
                        if not match:
                            print(f"[错误] {file_path}:{i+1} 信号和{target_type}参数顺序/类型不一致：")
                            print(f"  信号: {signal_class}::{signal_name}({', '.join(signal_params)})")
                            print(f"  {target_type}: {target_class}::{target_name}({', '.join(target_params)})")
                            print(f"  connect: {line.strip()}\n")

def scan_project():
    for root, dirs, files in os.walk(project_dir):
        for file in files:
            if file.endswith(('.h', '.cpp')):
                file_path = os.path.join(root, file)
                scan_signals_and_slots(file_path)

    for root, dirs, files in os.walk(project_dir):
        for file in files:
            if file.endswith(('.h', '.cpp')):
                file_path = os.path.join(root, file)
                scan_connect_calls(file_path)

if __name__ == '__main__':
    scan_project()
