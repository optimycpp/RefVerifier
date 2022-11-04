# `refverifier`: Gets all non-scalar parameters and checks for compilation errors.

## Building
```
git clone https://github.com/optimycpp/RefVerifier.git
cd RefVerifier
mkdir build && cd build
cmake ../llvm -G Ninja -DLLVM_ENABLE_PROJECTS="clang" -DLLVM_TARGETS_TO_BUILD=X86 -DLLVM_USE_SPLIT_DWARF=ON -DLLVM_OPTIMIZED_TABLEGEN=ON
ninja clang refverifier
```
The above command will build the tools in `build/bin` folder.

## Using pre-built docker image

Refer [docker_image](./docker_image) folder for details on using pre-built docker image.

## Using the tool

The `refverifier` tool has 2 functions, (1) To dump function info (i.e., functions with non-reference non-scalar parameters); (2) To check for compilation errors.

### Help
Getting help.
```
refverifier --help 

OVERVIEW: refverifier: Rewrite/verify const references.

USAGE: refverifier [options] <source0> [... <sourceN>]

OPTIONS:

Generic Options:

  --help                      - Display available options (--help-hidden for more)
  --help-list                 - Display list of available options (--help-list-hidden for more)
  --version                   - Display the version of this program

refverifier options:

  --comperror=<string>        - Path to the file where all compilation errors should be stored.
  --dumperror                 - Dump Compilation Errors
  --dumpfinfo                 - Dump Function Info
  --extra-arg=<string>        - Additional argument to append to the compiler command line
  --extra-arg-before=<string> - Additional argument to prepend to the compiler command line
  --funcinfo=<string>         - Path to the file containing all the function info
  -p=<string>                 - Build path
  --verbose                   - Print verbose information

-p <build-path> is used to read a compile command database.

	For example, it can be a CMake build directory in which a file named
	compile_commands.json exists (use -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	CMake option to get this output). When no build path is specified,
	a search for compile_commands.json will be attempted through all
	parent paths of the first input file . See:
	https://clang.llvm.org/docs/HowToSetupToolingForLLVM.html for an
	example of setting up Clang Tooling on a source tree.

<source0> ... specify the paths of source files. These paths are
	looked up in the compile command database. If the path of a file is
	absolute, it needs to point into CMake's source tree. If the path is
	relative, the current working directory needs to be in the CMake
	source tree and the file must be in a subdirectory of the current
	working directory. "./" prefixes in the relative files will be
	automatically removed, but the rest of a relative path must be a
	suffix of a path in the compile command database.

```

### Dumping Function Info

You can use the tool as regular compiler with the options.

```
./refverifier -dumpfinfo <source_file>
```

or 

```
./refverifier -dumpfinfo -p <build-path-containing compile_commands.json>
```

This will create `FuncInfo.json` with all the required info. You can specify a different file name using : `--funcinfo=` option.

Example:

```
./refverifier -dumpfinfo utils/tests/simple2.cpp
```
This will dump `FuncInfo.json` with various information. 

A snippet of it is as shown below:

```
...
{
            "Defn": {
                "BaseClass": "",
                "EndLineNo": 45,
                "FileName": "/projects/RefVerifier/clang/tools/refverifier/utils/tests/simple2.cpp",
                "FuncName": "help",
                "IsClassMethod": false,
                "IsDefinition": true,
                "IsStatic": true,
                "IsVirtual": false,
                "ParentClass": "",
                "StartLineNo": 41
            },
            "ID": 2365,
            // These are the parameters that are non-constant and non-scalar
            // This is a dictionary, which contains one entry for each parameter by its index.
            // For every parameter, there can be multiple entries (i.e., those beloning to declaration and definition)
            // Each entry has 5 fiels including File name and the following entries: 
            // where (TypeIndexLine, TypeIndexColumn) is the starting line and starting column of the parameter
            // and (ParamIndexLine, ParamIndexColumn) is the starting line and column of the parameter variable name.
            //Example:
            // static int help(class person 
            //                 obj, int i)
            // (41, 17)-> class person 
            // (42, 17)-> obj
            "NonConstNonScalarParams": {
                "0": [
                    {
                        "FileName": "/projects/RefVerifier/clang/tools/refverifier/utils/tests/simple2.cpp",
                        "ParamIndexColumn": 17,
                        "ParamIndexLine": 42,
                        "TypeIndexColumn": 17,
                        "TypeIndexLine": 41
                    }
                ],
                "1": [
                    {
                        "FileName": "/projects/RefVerifier/clang/tools/refverifier/utils/tests/simple2.cpp",
                        "ParamIndexColumn": 17,
                        "ParamIndexLine": 43,
                        "TypeIndexColumn": 22,
                        "TypeIndexLine": 42
                    }
                ]
            }

        }

...
```

### Dumping Compilation Error
```
./refverifier -dumperror <..source files>
```
This will dump `CompilationErrors.json` that contains compilation errrors organized according to functions.
You can specify a different file name using : `--comperror=` option.

Example:
```
./refverifier -dumperror utils/tests/simple2err.cpp

{"ErrorInfo":[{"FileName":"/home/machiry/projects/RefVerifier/clang/tools/refverifier/utils/tests/simple2err.cpp", "Errs":[{"LineNo":22, "Message":"cannot assign to variable 'obj' with const-qualified type 'const class person2 &'"}]}]}
```
