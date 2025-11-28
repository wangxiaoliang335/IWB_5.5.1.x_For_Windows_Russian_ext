# 解决 IWB 子模块问题

## 问题说明

`IWB` 目录显示为 "modified content"，这表明它是一个嵌套的 Git 仓库（子模块）。Git 无法直接提交子模块的内容。

## 解决方案

### 方案 1: 完全忽略 IWB 目录（推荐，如果不需要提交 IWB）

```bash
# 1. 从 Git 跟踪中移除 IWB（如果已经跟踪）
git rm --cached IWB

# 2. 添加到 .gitignore
echo "IWB/" >> .gitignore

# 3. 提交
git add .gitignore
git commit -m "Ignore IWB directory (nested git repository)"

# 4. 推送
git push github-ext master
```

### 方案 2: 将 IWB 作为子模块添加（如果 IWB 有独立的仓库）

如果您有 IWB 的 Git 仓库地址：

```bash
# 1. 移除当前跟踪
git rm --cached IWB

# 2. 删除 IWB 的 .git 目录（如果存在）
rm -rf IWB/.git

# 3. 添加 IWB 的所有内容
git add IWB
git commit -m "Add IWB directory content"

# 4. 推送
git push github-ext master
```

### 方案 3: 移除 IWB 的 .git 目录后添加（最简单）

如果不需要保留 IWB 作为独立的 Git 仓库：

```bash
# 1. 移除 IWB 的 .git 目录（保留所有文件）
rm -rf IWB/.git

# 2. 确保 IWB/.git/ 在 .gitignore 中（已经添加了）

# 3. 添加 IWB 目录
git add IWB

# 4. 提交
git commit -m "Add IWB directory (removed nested .git)"

# 5. 推送
git push github-ext master
```

## 推荐操作

**我推荐方案 3**，因为：
- 最简单直接
- 不需要额外的 Git 配置
- 可以正常提交 IWB 的所有内容
- 只是移除了嵌套的 Git 仓库，所有文件都保留

## 完整命令序列（方案 3）

```bash
# 1. 移除 IWB 的 .git 目录
rm -rf IWB/.git

# 2. 添加 IWB 目录
git add IWB

# 3. 提交
git commit -m "Add IWB directory (removed nested .git)"

# 4. 推送
git push github-ext master
```

## 注意事项

- `rm -rf IWB/.git` 只会删除 Git 元数据，**所有源代码文件都会保留**
- 移除后，IWB 不再是独立的 Git 仓库
- 如果将来需要，可以重新初始化 Git 仓库

