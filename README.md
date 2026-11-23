# Forth Interpreter

A C++17 implementation of an interactive, stack-based Forth interpreter. It includes a dynamically populated dictionary, word definitions, arithmetic and logic operations, basic memory access, and compiled control-flow structures.

## Features

- Interactive REPL prompt
- Integer data stack
- Custom word definitions
- Built-in stack-manipulation words
- Arithmetic, comparisons, and bitwise operations
- Basic memory reservation and direct memory access
- Control flow for `IF`, `DO LOOP`, `BEGIN UNTIL`, and `BEGIN WHILE REPEAT`
- Dictionary-based execution and compilation
- Clear interpreter error messages

## Requirements

- C++17-compatible compiler
- CMake 3.25 or newer
- Ninja (used by the supplied CMake preset)

On Windows, Visual Studio or the Visual Studio Build Tools must provide a C++ compiler and the environment from which CMake is invoked must be able to find it.

## Build

From the project root:

```powershell
cmake -S . -B build
cmake --build build --config Debug
```

You can also open the repository in an IDE that supports CMake and build the `ForthInterpreter` target.

## Run

```powershell
.\build\ForthInterpreter.exe
```

The interpreter prints `>> ` for each input line. Enter `bye` to exit.

```text
>> 2 3 + .
5 ok
>> bye
```

## Usage

Forth uses postfix notation: operators follow their operands.

```text
>> 10 3 +
ok
>> .
13 ok
```

Values are stored on a 256-entry data stack. Every successful operation reports `ok`.

### Defining Words

Words are defined between `:` and `;`.

```text
: square dup * ;
>> 7 square .
49 ok
```

The semicolon `;` is an immediate word and ends compilation.

### Stack Operations

| Word | Action | Stack effect |
|---|---|---|
| `drop` | Remove the top value | `( x -- )` |
| `dup` | Duplicate the top value | `( x -- x x )` |
| `swap` | Exchange the top two values | `( x1 x2 -- x2 x1 )` |
| `rot` | Rotate the top three values | `( x1 x2 x3 -- x2 x3 x1 )` |
| `over` | Copy the second value to the top | `( x1 x2 -- x1 x2 x1 )` |

### Arithmetic and Logic

| Word | Operation |
|---|---|
| `+` | Addition |
| `-` | Subtraction |
| `*` | Multiplication |
| `/` | Integer division |
| `mod` | Remainder |
| `=` | Equal; produces `-1` or `0` |
| `<` | Less than; produces `-1` or `0` |
| `>` | Greater than; produces `-1` or `0` |
| `negate` | Negate a value |
| `invert` | Bitwise complement |
| `and` | Bitwise AND |
| `or` | Bitwise OR |

Forth boolean values use `-1` for true and `0` for false.

### Output

| Word | Description |
|---|---|
| `.` | Print and remove the top stack value |
| `emit` | Print the top value as a character |
| `cr` | Print a newline |
| `." text"` | Print a string |

```text
>> ." Hello, Forth!"
Hello, Forth! ok
>> 65 emit
A ok
>> 65 .
65 ok
>> cr
<newline>
```

### Control Flow

Control-flow words must be used while compiling a word.

```text
: positive?
  dup 0 > if ." positive" else ." not positive" then ;
```

Supported loop structures include:

```text
: countdown
  5 0 do i . loop ;
```

```text
: until-example
  0 begin dup . 1 + dup 3 = until drop ;
```

The loop index is available through `I`.

### Memory

The interpreter provides a 4 KiB byte memory area and 64-bit integer cells.

| Word | Description |
|---|---|
| `here` | Push the next free memory address |
| `cell` | Convert a cell count to a byte count |
| `cell+` | Add the size of one cell to an address |
| `@` | Fetch a cell from an address |
| `!` | Store a cell at an address |

```text
0 42 ! 0 @ .
42 ok
```

`@` and `!` require valid operations within the 4 KiB memory area. Memory access outside the valid address range is rejected.

## Architecture

Key components:

- `FtInterpreter`: initializes the dictionary and evaluates REPL input
- `FtWordDictionary`: stores built-in and user-defined words
- `FtWord`: contains executable word behavior and compiled word data
- `FtStack`: stores up to 256 `long long` values
- `FtMemory`: provides a 4 KiB memory area
- `FtRuntimeContext`: manages stack, memory, output, and control-flow state
- `FtCompileContext`: tracks active definitions and loop frames

## Error Handling

The interpreter reports errors such as:

- Unknown words
- Invalid number formats
- Division by zero
- Empty or full stacks
- Invalid memory addresses
- Unterminated control-flow structures
- Use of compile-only words outside a definition

When an error occurs, compilation of the current word is aborted and the interpreter returns to the REPL.

## Implementation Notes

- Numbers are represented as `long long` cells.
- Integer division and modulo use C++ integer semantics.
- The stack capacity is 256 cells.
- The memory area is 4 KiB.
- Only unsigned decimal input is currently recognized by the number parser.
- `DO LOOP`, `BEGIN UNTIL`, `IF ELSE THEN`, and `BEGIN WHILE REPEAT` are compilation-oriented control structures.

## License

This project is licensed under the [Apache License 2.0](LICENSE.md).
