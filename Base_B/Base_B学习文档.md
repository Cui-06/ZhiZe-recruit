# Base_B 学习文档：Git 的使用

## 1. 学习目标

本题要求学习 Git 的基本使用方法，能够完成代码拉取、提交、推送，理解工作区、暂存区、本地仓库和远程仓库之间的关系，并能使用分支进行简单协作开发。

参考学习资料：

- 廖雪峰 Git 教程：https://liaoxuefeng.com/books/git/
- Learn Git Branching：https://learngitbranching.js.org/?locale=zh_CN

通过本次学习，我主要掌握以下内容：

1. Git 的基本概念和工作流程。
2. Git 仓库的创建、克隆、提交和推送。
3. 工作区、暂存区、本地仓库、远程仓库之间的关系。
4. 分支创建、切换、合并和删除。
5. 查看修改记录和提交历史。
6. 处理简单的合并冲突。
7. 编写 `.gitignore` 和 `README.md`。
8. 规范提交代码和提交信息。

## 2. Git 的作用

Git 是一个分布式版本控制系统，主要用于管理代码和文档的版本变化。它可以记录每一次修改，使项目能够回退到历史版本，也方便多人协作开发。

Git 的主要作用包括：

- 记录文件修改历史。
- 比较不同版本之间的差异。
- 回退到某个历史版本。
- 使用分支并行开发不同功能。
- 与 GitHub、Gitee 等远程平台配合，实现代码备份和协作。

Git 和普通文件备份不同。普通备份通常是复制多个文件夹，而 Git 会以提交为单位记录项目状态，每个提交都有对应的作者、时间、说明和唯一编号。

## 3. Git 基本概念

### 3.1 工作区

工作区就是当前项目文件夹中能直接看到和编辑的文件，例如 `.cpp`、`.py`、`.md` 等。平时写代码、改文档，都是在工作区中进行。

常用命令：

```bash
git status
```

`git status` 可以查看工作区中哪些文件被修改、哪些文件还没有被 Git 跟踪。

### 3.2 暂存区

暂存区用于临时保存准备提交的修改。修改文件后，需要先使用 `git add` 把改动加入暂存区，然后再提交。

常用命令：

```bash
git add 文件名
git add .
```

其中 `git add .` 表示把当前目录下的修改统一加入暂存区。

### 3.3 本地仓库

本地仓库是保存在自己电脑上的 Git 仓库。执行 `git commit` 后，暂存区中的内容会被提交到本地仓库，形成一个新的版本记录。

常用命令：

```bash
git commit -m "提交说明"
```

### 3.4 远程仓库

远程仓库通常托管在 GitHub、Gitee 等平台上，用于备份代码和多人协作。

常用命令：

```bash
git remote -v
git push
git pull
```

`git push` 用于把本地提交推送到远程仓库，`git pull` 用于把远程仓库的最新内容拉取到本地。

### 3.5 四个区域的关系

Git 的基本流程可以理解为：

```text
工作区 -> git add -> 暂存区 -> git commit -> 本地仓库 -> git push -> 远程仓库
远程仓库 -> git pull -> 本地工作区
```

表格总结如下：

| 区域 | 含义 | 常用命令 |
| --- | --- | --- |
| 工作区 | 当前正在编辑的项目文件 | `git status` |
| 暂存区 | 准备提交的修改 | `git add` |
| 本地仓库 | 本机保存的版本记录 | `git commit` |
| 远程仓库 | GitHub/Gitee 上的仓库 | `git push`、`git pull` |

## 4. Git 基础命令总结

### 4.1 初始化仓库

如果当前文件夹还不是 Git 仓库，可以使用：

```bash
git init
```

执行后，当前目录下会生成一个隐藏的 `.git` 文件夹，用来保存 Git 仓库信息。

### 4.2 克隆远程仓库

如果远程平台已经有仓库，可以使用 `git clone` 下载到本地。

```bash
git clone 仓库地址
```

例如：

```bash
git clone https://github.com/用户名/仓库名.git
```

克隆完成后，本地会生成一个完整项目文件夹，并自动关联远程仓库。

### 4.3 查看仓库状态

```bash
git status
```

该命令可以查看：

- 哪些文件被修改。
- 哪些文件未被跟踪。
- 哪些文件已经加入暂存区。
- 当前所在分支。

这是使用 Git 时最常用的检查命令。

### 4.4 添加到暂存区

```bash
git add 文件名
git add .
```

如果只想提交某个文件，可以使用 `git add 文件名`。如果想提交当前目录下所有修改，可以使用 `git add .`。

### 4.5 提交到本地仓库

```bash
git commit -m "完成 Git 基础学习文档"
```

提交说明应该简洁明确，能看出本次提交做了什么。不要使用太模糊的说明，例如 `update`、`test`、`修改`。

较好的提交说明示例：

```bash
git commit -m "docs: add Base_B Git learning notes"
git commit -m "fix: correct branch merge example"
git commit -m "feat: add rational number class"
```

### 4.6 拉取远程代码

```bash
git pull
```

`git pull` 会从远程仓库获取最新提交，并合并到当前本地分支。多人协作时，开始写代码前通常先执行 `git pull`，避免本地代码落后太多。

### 4.7 推送到远程仓库

```bash
git push
```

如果是第一次推送当前分支，可能需要指定远程仓库和分支：

```bash
git push -u origin main
```

其中 `origin` 是远程仓库默认名称，`main` 是分支名称。

## 5. 分支的使用

### 5.1 分支的意义

分支可以让不同任务互不影响。例如主分支 `main` 保持稳定，开发新功能时创建新的功能分支，完成后再合并回主分支。

常见分支使用方式：

```text
main：稳定版本
feature/git-note：编写 Git 学习文档
fix/readme-error：修复 README 错误
```

### 5.2 查看分支

```bash
git branch
```

当前所在分支前面会有 `*` 标记。

### 5.3 创建并切换分支

```bash
git switch -c feature/base-b
```

也可以使用旧命令：

```bash
git checkout -b feature/base-b
```

### 5.4 切换分支

```bash
git switch main
```

或者：

```bash
git checkout main
```

### 5.5 合并分支

在 `feature/base-b` 分支完成修改后，可以切回 `main` 分支并合并：

```bash
git switch main
git merge feature/base-b
```

如果两个分支修改了不同文件，通常可以自动合并。如果修改了同一文件的同一位置，可能产生冲突，需要手动处理。

### 5.6 删除分支

分支合并后，可以删除不再需要的本地分支：

```bash
git branch -d feature/base-b
```

如果分支没有合并，Git 会阻止删除，避免误删未保存的开发内容。

## 6. 查看修改记录

### 6.1 查看提交历史

```bash
git log
```

`git log` 会显示提交记录，包括提交编号、作者、时间和提交说明。

简洁显示方式：

```bash
git log --oneline
```

示例输出：

```text
a1b2c3d docs: add Base_B Git learning notes
f4e5d6c init project
```

### 6.2 查看文件差异

查看工作区和暂存区之间的差异：

```bash
git diff
```

查看暂存区和上一次提交之间的差异：

```bash
git diff --cached
```

这些命令可以帮助确认自己到底修改了什么，避免提交不必要的内容。

### 6.3 查看某次提交内容

```bash
git show 提交编号
```

该命令可以查看某次提交修改了哪些文件、具体改动了哪些内容。

## 7. 冲突处理

### 7.1 冲突产生原因

当两个分支或两个人修改了同一个文件的同一位置时，Git 可能无法自动判断应该保留哪一份内容，这时就会产生冲突。

例如，一个文件中可能出现：

```text
<<<<<<< HEAD
当前分支的内容
=======
要合并进来的内容
>>>>>>> feature/base-b
```

### 7.2 冲突处理步骤

处理冲突的一般流程：

1. 使用 `git status` 查看冲突文件。
2. 打开冲突文件，找到 `<<<<<<<`、`=======`、`>>>>>>>` 标记。
3. 根据实际需要保留正确内容，删除冲突标记。
4. 使用 `git add` 标记冲突已解决。
5. 使用 `git commit` 完成合并提交。

命令示例：

```bash
git status
git add 冲突文件
git commit
```

在 VSCode 中，冲突文件会有明显提示，可以选择保留当前更改、传入更改或同时保留，再手动检查最终内容。

## 8. `.gitignore` 的编写

`.gitignore` 用于告诉 Git 哪些文件不需要被版本控制。例如编译产物、临时文件、缓存文件、日志文件等，一般不应该提交到仓库。

常见 `.gitignore` 示例：

```gitignore
# 编译输出
build/
dist/
*.exe
*.o

# Python 缓存
__pycache__/
*.pyc

# 日志文件
*.log

# IDE 配置
.vscode/
.idea/

# 系统文件
.DS_Store
Thumbs.db
```

需要注意：如果某个文件已经被 Git 跟踪，后来再写入 `.gitignore`，Git 不会自动停止跟踪它。需要先从 Git 跟踪中移除：

```bash
git rm --cached 文件名
```

## 9. `README.md` 的编写

`README.md` 是项目说明文件，通常放在仓库根目录。它是别人了解项目的入口，也能帮助自己整理项目结构和运行方式。

一个基础 `README.md` 可以包含：

```markdown
# 项目名称

## 项目简介

说明这个项目实现了什么功能。

## 环境要求

- 操作系统
- 编程语言版本
- 依赖库

## 运行方法

```bash
运行命令
```

## 目录结构

说明主要文件夹和文件的作用。

## 学习记录

记录学习过程和关键知识点。
```

写 README 时应注意：

- 标题清楚。
- 运行步骤完整。
- 依赖环境写明。
- 文件结构简洁。
- 不只写结果，也要写必要说明。

## 10. Git 常用工作流程

### 10.1 单人开发流程

```bash
git status
git add .
git commit -m "docs: update Git learning notes"
git push
```

适合个人项目或学习文档提交。

### 10.2 多人协作流程

```bash
git pull
git switch -c feature/new-task
git add .
git commit -m "feat: complete new task"
git switch main
git pull
git merge feature/new-task
git push
```

多人协作时，开始开发前先拉取远程最新代码；开发新任务时使用独立分支；合并前再次同步主分支，减少冲突。

## 11. 规范提交代码

规范提交不仅是执行 `git commit`，还包括提交前检查和清晰的提交说明。

提交前应该检查：

```bash
git status
git diff
```

确认内容无误后再执行：

```bash
git add .
git commit -m "类型: 提交内容说明"
```

常见提交类型：

| 类型 | 含义 |
| --- | --- |
| `feat` | 新功能 |
| `fix` | 修复问题 |
| `docs` | 文档修改 |
| `style` | 格式调整 |
| `refactor` | 代码重构 |
| `test` | 测试相关 |
| `chore` | 构建、配置等杂项 |

示例：

```bash
git commit -m "docs: add Base_B Git learning document"
git commit -m "feat: implement rational number operations"
git commit -m "fix: handle division by zero"
```
