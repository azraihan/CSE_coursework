## Prerequisite Tools:
[pdos MIT CSAIL](https://pdos.csail.mit.edu/6.828/2022/tools.html)

## Cloning Codebase: 
```bash
git clone https://github.com/mit-pdos/xv6-riscv
```

## Applying Patch: 

```bash
git apply --whitespace=fix <patch file name> 
```
e.g.: (put the patch file in the same directory as the xv6 directory)
```bash
cd xv6-riscv
git apply --whitespace=fix 2105009.patch
```

## Compile and Run (from inside xv6-riscv directory): 
```bash
make clean; make qemu
```