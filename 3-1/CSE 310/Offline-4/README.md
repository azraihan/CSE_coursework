### Antlr4 Installation Guide:
[Installtion Guide PDF](https://github.com/azraihan/CSE_coursework/blob/main/3-1/CSE%20310/Offline-3/Getting_Started_with_ANTLR4.pdf)

### Running The Script

```bash
bash run-script.sh input/<input_file_name>
```

e.g.
```bash
bash run-script.sh input/test1_i.c
```

The output will be found in the `output/` directory:
- `code.asm`: contains the generated assembly code
- `optcode.asm`: contains the optimized assembly code after some peephole optimizations

### Cleaning The Outputs

```bash
bash clean-script.sh
```