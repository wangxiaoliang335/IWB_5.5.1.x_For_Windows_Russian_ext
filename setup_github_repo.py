#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
设置GitHub远程仓库脚本
用于添加新的GitHub远程仓库并配置Git仓库
"""

import subprocess
import sys
import os

def run_command(cmd, check=True):
    """运行命令并返回结果"""
    try:
        print(f"执行: {cmd}")
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True, encoding='utf-8', errors='ignore')
        if result.stdout:
            print(result.stdout)
        if result.stderr and result.returncode != 0:
            print(f"错误: {result.stderr}", file=sys.stderr)
        if check and result.returncode != 0:
            sys.exit(1)
        return result
    except Exception as e:
        print(f"执行命令时出错: {e}", file=sys.stderr)
        if check:
            sys.exit(1)
        return None

def check_git_installed():
    """检查Git是否已安装"""
    result = run_command("git --version", check=False)
    if result and result.returncode == 0:
        print(f"✓ Git已安装: {result.stdout.strip()}")
        return True
    else:
        print("✗ 未找到Git，请先安装Git")
        return False

def init_git_repo():
    """初始化Git仓库（如果还没有）"""
    if os.path.exists(".git"):
        print("✓ Git仓库已存在")
        return False
    else:
        print("初始化Git仓库...")
        run_command("git init")
        print("✓ Git仓库初始化成功")
        return True

def check_remote_exists(remote_name):
    """检查远程仓库是否存在"""
    result = run_command(f"git remote get-url {remote_name}", check=False)
    return result and result.returncode == 0

def add_remote_repo(remote_name, remote_url):
    """添加或更新远程仓库"""
    if check_remote_exists(remote_name):
        print(f"远程仓库 '{remote_name}' 已存在，正在更新...")
        run_command(f"git remote set-url {remote_name} {remote_url}")
    else:
        print(f"添加新的远程仓库 '{remote_name}'...")
        run_command(f"git remote add {remote_name} {remote_url}")
    print(f"✓ 远程仓库 '{remote_name}' 已配置: {remote_url}")

def show_remotes():
    """显示所有远程仓库"""
    print("\n当前远程仓库配置:")
    print("-" * 60)
    run_command("git remote -v")
    print("-" * 60)

def main():
    print("=" * 60)
    print("设置GitHub远程仓库")
    print("=" * 60)
    print()
    
    # 检查Git是否安装
    if not check_git_installed():
        sys.exit(1)
    
    print()
    
    # 初始化Git仓库
    print("[1/3] 检查Git仓库状态...")
    was_init = init_git_repo()
    
    print()
    
    # 显示当前远程仓库
    print("[2/3] 查看当前远程仓库配置...")
    show_remotes()
    
    print()
    
    # 添加新的远程仓库
    print("[3/3] 添加新的GitHub远程仓库...")
    remote_name = "github"
    remote_url = "git@github.com:wangxiaoliang335/IWB_5.5.1.x_For_Windows_Russian.git"
    add_remote_repo(remote_name, remote_url)
    
    print()
    
    # 显示更新后的远程仓库
    print("更新后的远程仓库配置:")
    show_remotes()
    
    print()
    print("=" * 60)
    print("完成！")
    print("=" * 60)
    print()
    print("下一步操作:")
    print("1. 使用 'git remote -v' 查看所有远程仓库")
    print("2. 使用 'git add .' 添加文件")
    print("3. 使用 'git commit -m \"Your message\"' 提交更改")
    print(f"4. 使用 'git push {remote_name} main' 或 'git push {remote_name} master' 推送到GitHub")
    print()
    print("注意: 确保已配置SSH密钥以使用git@github.com地址")
    print("     或使用HTTPS地址: https://github.com/wangxiaoliang335/IWB_5.5.1.x_For_Windows_Russian.git")
    print()

if __name__ == "__main__":
    main()

