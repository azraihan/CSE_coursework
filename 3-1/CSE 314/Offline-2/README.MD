## Prerequisite Tools:
[pdos MIT CSAIL](https://pdos.csail.mit.edu/6.828/2022/tools.html)

## Cloning codebase: 
```bash
git clone https://github.com/mit-pdos/xv6-riscv
```

## Applying patch: 

```bash
git apply --whitespace=fix <patch file name> 
#e.g.: (put the patch file in the same directory as the xv6 directory)
cd xv6-riscv
git apply --whitespace=fix 2105009.patch
```

## Compile and run (from inside xv6-riscv directory): 
```bash
make clean; make qemu
```