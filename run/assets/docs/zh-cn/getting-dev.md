

# Getting Dev

本文将介绍 配置开发环境, 及*Github贡献方法*  

[[VIDEO]]()

## IDE

我在用 [CLion 2020.3.4 (官网下载)](https://www.jetbrains.com/clion/download/other.html), 
因为他可以用 [eval-reset.bat]() 脚本无限重制试用期.
另外你可以使用 [Visual Studio 2019 Dark 主题](https://plugins.jetbrains.com/plugin/14965-visual-studio-2019-dark-theme) 如果你喜欢VS的界面风格.

启动CLion后, 进入设置 'Settings > Toolchain' 添加我们下载的 ucrt-mingw64 环境. (仅Windows需要这样做)  
另外 Ethertia 的运行目录应该在 `$ProjectFileDir$/run`




## How to Contribute

Pull Request

### Commit Formats

在commit时使用以下前缀 表示变更的内容类型:

| Type Prefix | Meaning | 
| ----   | --- |
| feat   | New Feature |
| fix    |  |
| fix-to | bugfix, 多次提交修复问题 |
| docs   |    |
| style  | 格式 (不影响代码运行) |
| refactor  | 重构 |
| optim  | 性能优化 |
| test   | 增加测试 |
| chore  | 构建过程或辅助工具的变动
| revert | 回滚到之前的版本 |

e.g.: `feat: full-display game viewport`