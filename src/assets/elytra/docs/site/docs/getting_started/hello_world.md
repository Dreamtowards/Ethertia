

# Hello Word™   

In this article, we'll see how to setup Elyrta SDK on your System & make a "Hello World" program.  
Elytra SDK contains Runtime, Compiler, Libraries, DevTools(Package Management Tool, etc). 
for details, see [SDK Overview]().  
If you want to quick try Elytra online without any download/setup, 
[Elytra Playground]() is for you. (limited functionality)

## 1. Download SDK for your System

<a class="btn btn-primary text-white" href="https://elytra.dev/sdk-latest">Download SDK</a>


Choose a SDK capable for your System

Supported Systems:  
 - Windows (7, 10, 11 | x86 64bit, arm64)  
 - Linux  
 - MacOSX (10.15 'Catalina', 11 'Big Sur', 12 'Monterey')  


## 2. Unzip & Setup the SDK

```shell
$ tar -xvf ./elytra-sdk-0.1.4-macosx-x64-stable.zip
```

After the SDK is downloaded, 

> _Why no Installer?:_ we like all in the control. manually install could be simple and clear.


> __Tip: Need add `bin/elytra` to your Console commands.?__
>
> __Linux / MacOS:__  
> add e.g. `export PATH="$PATH:/home/enderman/elytra-sdk/bin"` to your shell's "run commands" file.  
> _(Bash shell: `~/.bashrc`, Z Shell: `~/.zshrc`, Korn Shell: `~/.kshrc`)_
>
> __Windows:__  
> add e.g. `C:/Users/Demo/elytra-sdk/bin` to your _Environment Variable_ -> _Path_
>
> Don't forget change the demo-sdk-path to your actual path.  

## 3. run `Hello World`

Source file `Test.et`:
```elytra
using std::io::Console;

void main() {
    
    Console::writeln("Hello Word");   
}
```

Terminal run:
```shell
$ ./elytra-sdk/bin/elytra build ./test.et
$ ./elytra-sdk/bin/elytra run ./test.o
Hello Word
```






<!--


<div class="ac-tip" markdown="1">
    <p>Wanna try Elytra on the fly?</p>
    
[Elyrta Playground](): try Elytra online, without download/install.
</div>


<div class="ac-tip" markdown="1">
    <p>Want add `bin/elytra` to your Console commands.? </p>

__Linux / MacOS:__  
add e.g. `export PATH="$PATH:/home/enderman/elytra-sdk/bin"` to your shell's "run commands" file.  
_(Bash shell: `~/.bashrc`, Z Shell: `~/.zshrc`, Korn Shell: `~/.kshrc`)_

__Windows:__  
add e.g. `C:/Users/Demo/elytra-sdk/bin` to your _Environment Variable_ -> _Path_

Don't forget change the demo-sdk-path to your actual path.
</div>


<div class="ac-tip" markdown="1">
    <p>What inside the SDK?</p>

- Runtime
- Compiler
- _Package Management Tool?_
</div>

## Appendixes & Tips

#### Wanna try Elytra on the fly?
[Elytra Playground](): try Elytra online, without download/install.
  
#### What inside the SDK?

- Runtime
- Compiler
- _Package Management Tool?_

#### Need add `bin/elytra` to your Console command.? 
__Linux / MacOS:__  
add e.g. `export PATH="$PATH:/home/enderman/elytra-sdk/bin"` to your shell's "run commands" file.  
_(Bash shell: `~/.bashrc`, Z Shell: `~/.zshrc`, Korn Shell: `~/.kshrc`)_

__Windows:__  
add e.g. `C:/Users/Demo/elytra-sdk/bin` to your _Environment Variable_ -> _Path_

Don't forget change the demo-sdk-path to your actual path.

<br><br>

<!--

# Title Level1

## Title Level2

### title level3



#### Polls

<small>*prim*</small>

- `220331` [rdv/22u13b Lib&Performance. use GLFW,lwjgl331,jdk16.]()
- `220331` [rdv/22u13a Gui LazyRendering.]()

```shell
code sec
```

***L3***

- Cradle Waltz  
  <small>FLuoRiTe r. M2U</small>


- Gymnopedie no.1  
  <small>Blüchel & Von Deylen . Bi Polar</small>

***L2***

- Euphoria <i>(Originally Performed by BTS) (Piano Karaoke Version)</i>  
  <small>Sing2Piano - Euphoria (Piano Karaoke Instrumentals)</small>


- CREAM STEW (Piano ver.)  
  <small>FLouRiTe , Aioi</small>
  
-->