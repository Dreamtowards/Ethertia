

# Hello World sample

```
using std::io::Console;

void main() {

    Console::writeln("Hello World", "a", 1, 2);  // concat the args.
}
```

> __The "main" function: Args & Exit code.__
> 
> 您可能注意到了，不像C,Java。这里的入口函数没有args，也没有结束码。而是一个简单纯粹的'过程'函数(无参无反)。  
> 然而对应的功能并没有少，您仍可无损地调用它们。 - 只是调用它们方式变得通用了。像使用其他无尽的功能一样使用它们。  
> "更少即是更多" 或许可以玩笑般地用在这里。
> ```
> using std::System;
> using std::util::ArrayList;
> 
> void main() {
>     ArrayList<std::String> args = System::args();  
>     // lazy args. could be a spacial opt.
>     
>     std::exit(4);
>     // if not exit() call, default exit code will be 0.
> }
> ```

> __The `::` symbol-scope-resolution notation.__  
> 在第1和第5行，您可能已经注意到了 `::` 这个记号。它是 _符号域解析记号_，纯静态 无运行时开销。
>
> (符号(Symbol)：指一个对应的程序实体，例一个变量，函数，命名空间或类等。)
>
> [迷思] 在一些语言如Java,C#中，`.`(成员访问记号) 也被表示符号域解析，因此您可能困惑 为何不全用 `.`记号？
> 毕竟更少的语言记号 会让语言更简单不是吗。  
> 
> 但这里认为 成员访问 `.`, 应该和符号域解析 `::` 分开。因为前者是有运行时开销的，而后者没有。  
> 比如 `obj.var1` 访问obj的成员变量var1，运行时需将obj内部指针偏移到成员变量var1的地址。如果obj本身是指针 还需一次取值操作再偏移。
> 而 `obj.func1()` 成员函数调用，则需隐式地将obj的地址带入func1函数中。  
> 而符号域解析则无运行时开销，他只是告诉编译器你表示的符号。`Console::writeln()`就像`Console_writeln()`一样。
> 因此你在看到`::`时则无需顾虑开销问题。并且清楚地知道他的意图 - 很简单 表示符号 而没有背后的其他操作。
> 
> 我们追求简单和一致性，但这里认为 前提是没有混淆并表达清楚。而且`::`看上去更酷 - 至少四倍 (玩笑话)。