# Base_A 学习文档：Markdown 的使用

## 1. 学习目标

本题要求学习 Markdown 的基本用法，并能够使用 Markdown 撰写结构清晰的学习文档。根据题目给出的学习资料，我主要学习了 Markdown 的基础语法，以及部分可以在 Markdown 中使用的 HTML5 标签。

本次学习使用的工具是 VSCode。VSCode 可以直接编辑 `.md` 文件，并通过预览功能查看 Markdown 的渲染效果，适合边写边检查文档结构。

参考资料：

- Markdown 基础语法：https://markdown.com.cn/basic-syntax/

## 2. VSCode 使用记录

我使用 VSCode 打开并编辑 `Base_A学习文档.md` 文件，在编辑过程中主要使用了以下功能：

| 功能 | 作用 |
| --- | --- |
| Markdown 文件编辑 | 编写学习文档正文 |
| Markdown 预览 | 查看标题、表格、代码块等渲染效果 |
| 代码高亮 | 方便检查 Markdown、HTML、C++ 等代码块 |
| 文件资源管理器 | 管理题目文件夹和文档文件 |

在 VSCode 中，可以使用 `Ctrl + Shift + V` 打开 Markdown 预览，也可以使用 `Ctrl + K` 后再按 `V` 打开侧边预览。通过预览可以及时发现标题层级、列表缩进、表格格式是否正确。

## 3. Markdown 关键知识点总结

### 3.1 Markdown 的作用

Markdown 是一种轻量级标记语言，适合用来编写技术文档、学习笔记、项目说明和实验报告。它的特点是语法简单、可读性强，即使不渲染成网页，源文件本身也比较容易阅读。

Markdown 常见使用场景包括：

- GitHub 项目的 `README.md`
- 学习文档和技术报告
- 博客文章
- 项目说明书
- 实验过程记录

### 3.2 标题

Markdown 使用 `#` 表示标题，`#` 的数量表示标题层级。

````markdown
# 一级标题
## 二级标题
### 三三级标题
#### 四级标题
````

标题可以帮助文档形成清晰结构。一般一篇文档只使用一个一级标题，后续内容使用二级标题和三级标题展开。

### 3.3 段落与换行

Markdown 中段落之间需要用空行分隔。

````markdown
这是第一段内容。

这是第二段内容。
````

如果只是在源文件中普通换行，渲染时不一定会真正换行。需要强制换行时，可以在行尾添加两个空格，也可以使用 HTML 的 `<br>` 标签。

### 3.4 文本强调

Markdown 支持加粗、斜体和删除线等文本效果。

````markdown
**加粗**
*斜体*
***加粗并斜体***
~~删除线~~
````

效果示例：

- **加粗**
- *斜体*
- ***加粗并斜体***
- ~~删除线~~

在技术文档中，加粗通常用于突出关键词、重要结论或注意事项。

### 3.5 列表

Markdown 支持无序列表和有序列表。

无序列表：

````markdown
- 学习 Markdown 语法
- 使用 VSCode 编辑文档
- 检查预览效果
````

有序列表：

````markdown
1. 创建 Markdown 文件
2. 编写文档内容
3. 打开预览检查格式
````

列表适合用于整理步骤、知识点、任务要求等内容。

### 3.6 引用

引用使用 `>` 表示。

````markdown
> Markdown 适合编写结构清晰、便于维护的技术文档。
````

效果：

> Markdown 适合编写结构清晰、便于维护的技术文档。

引用可以用于摘录说明、强调观点或补充说明。

### 3.7 行内代码和代码块

行内代码使用一对反引号，适合标记命令、文件名、函数名、变量名等。

````markdown
使用 `Ctrl + Shift + V` 打开 Markdown 预览。
````

多行代码使用代码块，代码块建议标明语言类型，方便 VSCode 或 GitHub 进行语法高亮。

````markdown
```cpp
#include <iostream>

int main() {
    std::cout << "Hello Markdown" << std::endl;
    return 0;
}
```
````

效果示例：

```cpp
#include <iostream>

int main() {
    std::cout << "Hello Markdown" << std::endl;
    return 0;
}
```

代码块适合放源码、命令行记录、配置文件和运行日志。

### 3.8 链接

Markdown 链接格式如下：

````markdown
[链接文字](链接地址)
````

示例：

````markdown
[Markdown 基础语法](https://markdown.com.cn/basic-syntax/)
````

效果：

[Markdown 基础语法](https://markdown.com.cn/basic-syntax/)

使用带说明文字的链接比直接粘贴网址更清晰。

### 3.9 图片

Markdown 插入图片的语法是在链接语法前加 `!`。

````markdown
![图片说明](图片路径)
````

示例：

````markdown
![运行截图](./screenshots/result.png)
````

图片路径可以使用相对路径或网络路径。写项目文档时，相对路径更适合和项目文件一起提交。

### 3.10 表格

Markdown 表格由表头、分隔行和内容行组成。

````markdown
| 语法 | 作用 |
| --- | --- |
| `#` | 标题 |
| `**文本**` | 加粗 |
| `` `代码` `` | 行内代码 |
````

效果：

| 语法 | 作用 |
| --- | --- |
| `#` | 标题 |
| `**文本**` | 加粗 |
| `` `代码` `` | 行内代码 |

表格适合整理对比信息、参数说明和实验结果。

### 3.11 分隔线

分隔线可以使用三个或更多 `-`、`*` 或 `_`。

````markdown
---
````

效果：

---

分隔线可以用于区分不同内容块，但在结构清晰的文档中不需要频繁使用。

### 3.12 任务列表

任务列表常用于记录任务完成情况。

````markdown
- [x] 学习 Markdown 标题
- [x] 学习 Markdown 表格
- [ ] 补充图片示例
````

效果：

- [x] 学习 Markdown 标题
- [x] 学习 Markdown 表格
- [ ] 补充图片示例

任务列表在 GitHub 上支持较好，适合用于 README、项目计划和学习进度记录。

### 3.13 转义字符

当需要显示 Markdown 符号本身，而不是让它被解析成格式时，可以使用反斜杠 `\`。

````markdown
\# 这不是标题
\* 这不是列表
````

效果：

\# 这不是标题

\* 这不是列表

转义字符适合用于语法说明类文档。

## 4. Markdown 中的基础 HTML5

Markdown 支持嵌入部分 HTML 标签。HTML 可以补充 Markdown 原生语法的不足，例如控制换行、折叠内容、显示上标下标等。

### 4.1 换行标签 `<br>`

`<br>` 用于强制换行。

````html
第一行<br>
第二行
````

效果：

第一行<br>
第二行

### 4.2 折叠标签 `<details>` 和 `<summary>`

折叠块可以用于隐藏较长的补充内容。

````html
<details>
<summary>点击展开</summary>

这里是被折叠的内容。

</details>
````

效果：

<details>
<summary>点击展开</summary>

这里是被折叠的内容。

</details>

### 4.3 上标和下标

HTML 的 `<sup>` 表示上标，`<sub>` 表示下标。

````html
面积单位：m<sup>2</sup>
水的化学式：H<sub>2</sub>O
````

效果：

面积单位：m<sup>2</sup>

水的化学式：H<sub>2</sub>O

### 4.4 高亮文本

`<mark>` 可以表示高亮内容。

````html
<mark>重点内容</mark>
````

效果：

<mark>重点内容</mark>

### 4.5 键盘按键

`<kbd>` 适合表示快捷键。

````html
按 <kbd>Ctrl</kbd> + <kbd>S</kbd> 保存文件。
````

效果：

按 <kbd>Ctrl</kbd> + <kbd>S</kbd> 保存文件。

### 4.6 图片标签

Markdown 的图片语法不方便设置图片大小，可以使用 HTML 的 `<img>` 标签控制宽度。

````html
<img src="./screenshots/result.png" alt="运行截图" width="600">
````

常用属性：

| 属性 | 作用 |
| --- | --- |
| `src` | 图片路径 |
| `alt` | 图片说明 |
| `width` | 图片宽度 |
| `height` | 图片高度 |

## 5. Markdown 与 HTML5 的关系

Markdown 的优势是简洁，HTML 的优势是表达能力更强。在写学习文档时，应优先使用 Markdown 原生语法；当 Markdown 无法满足需求时，再少量使用 HTML 标签。

适合使用 Markdown 的内容：

- 标题
- 段落
- 列表
- 表格
- 链接
- 代码块

适合使用 HTML 补充的内容：

- 强制换行
- 折叠内容
- 上标和下标
- 图片大小控制
- 快捷键样式

需要注意的是，不同平台对 HTML 标签的支持程度不同。GitHub、VSCode 预览和部分在线 Markdown 编辑器的渲染效果可能不完全一致。
