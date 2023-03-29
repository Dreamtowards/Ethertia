

# Getting Dev

本文将介绍 配置开发环境, 及*Github贡献方法*  
自从您成功地[构建并运行]()后，欢迎来到开发

[[VIDEO]]()

## IDE

我在用 [CLion 2020.3.4 (官网下载)](https://www.jetbrains.com/clion/download/other.html), 
因为他可以用 [eval-reset.bat]() 脚本无限重制试用期.
另外你可以使用 [Visual Studio 2019 Dark 主题](https://plugins.jetbrains.com/plugin/14965-visual-studio-2019-dark-theme) 如果你喜欢VS的界面风格的话.

启动CLion后, 进入设置 'Settings > Toolchain' 添加我们下载的 ucrt-mingw64 环境. (仅Windows需要这样做)  
另外 Ethertia 的运行目录应该在 `$ProjectFileDir$/run`




## How to Contribute

如果您乐意的话，可以使用 Github 的 Pull Request 功能贡献代码。_(但在这么做之前 应该先明白您为什么要帮助我们 是否能从中获得您所需要的东西)_

### Pull Request

_我还不太清楚 git pr, 待补充_

### Commit formats

在commit时使用以下前缀 表示变更的内容类型:

| Type Prefix | 意思 | 
| ----   | --- |
| feat   | New Feature |
| fix    | bug修复 |
| fix-to | bugfix, 多次提交修复问题 |
| docs   | 文档相关  |
| style  | 格式 (不影响代码运行) |
| refactor  | 重构 |
| perf | 性能优化 |
| opti | 体验优化 |
| test   | 增加测试 |
| chore  | 构建过程或辅助工具的变动
| revert | 回滚到之前的版本 |

e.g.: `feat: add gui popup-menu 'New Entity' on Entities window`