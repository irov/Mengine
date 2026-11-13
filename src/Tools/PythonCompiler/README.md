# PythonCompiler

Compile source to a Python 2.7 `.pyo` file (magic, timestamp and marshal-v2):

```sh
PythonCompiler --in script.py --out script.pyo \
    -D_DEVELOPMENT=False -DAPI_VERSION=3 "-DCHANNEL='stable'"
```

`-D NAME=literal` is also accepted. Definitions use TinyPy's shared build-profile
parser. Values are explicit Python 2.7 literals: `None`, bool, integer/long,
float, quoted byte/Unicode strings and nested tuples. Keep literal quotes when
passing strings through the shell. Duplicate names and non-literal expressions
are errors; no Python code executes while definitions are parsed.

Definitions are independent of the tool's optimization level. Matching bare
names become typed constants throughout the script, including outside `if`.
Names are any Python identifiers and match lexically in every scope: a supplied
name cannot be assigned, imported or used as a parameter anywhere (so
`-Dself=...` rejects every method), it shadows a builtin of the same spelling,
and a misspelled reference stays an ordinary runtime name. Attributes and
string contents are unchanged. An invalid definition is reported by its text.
No `_DEVELOPMENT` or `__NDEBUG__` definition is supplied automatically. Without
a definition these remain runtime names. Only Python's builtin `__debug__`
retains its standard optimization-dependent behavior.

Validate source without writing bytecode:

```sh
PythonCompiler --in script.py --validate -D_DEVELOPMENT=False
```

Output compilation uses optimize level 1; validation uses level 0. Both use
the same explicit definitions, preprocessing and meta support. Neither command
changes the `.pyo` header or the Builder's subsequent `.pyz` compression format.
Invalid definitions leave an existing output file intact.
