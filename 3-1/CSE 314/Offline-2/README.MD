# XV6 Operating System Assignment

This assignment implements a **multi-level feedback queue scheduler** and **system call tracking** in the XV6 operating system.

## Features Implemented

- **Multi-Level Feedback Queue (MLFQ) Scheduler**
  - Lottery scheduling for high-priority queue
  - Round-robin scheduling for low-priority queue
  - Priority boosting mechanism
  - Configurable time slices and ticket counts

- **System Call Statistics**
  - Tracks call count and execution time for each system call
  - `history` command to view syscall statistics

- **Process Information**
  - `getpinfo` syscall to view process scheduling information
  - `settickets` syscall to set lottery tickets for processes

## Files Added/Modified

- `kernel/proc.c` - MLFQ scheduler implementation
- `kernel/syscall_stat.c` - System call tracking
- `kernel/random.c` - Random number generator for lottery
- `user/history.c` - Display syscall statistics
- `user/testprocinfo.c` - Test process information
- `user/dummyproc.c` - Test process for scheduler

## Setup Instructions

### Prerequisites
Install required tools: [MIT PDOS Tools](https://pdos.csail.mit.edu/6.828/2022/tools.html)

### Installation

1. **Clone XV6 codebase:**
```bash
git clone https://github.com/mit-pdos/xv6-riscv
```

2. **Apply the patch:**
```bash
cd xv6-riscv
git apply --whitespace=fix 2105009.patch
```

3. **Compile and run:**
```bash
make clean; make qemu
```

## Usage

Once XV6 is running, try these commands:
- `history` - View all syscall statistics
- `history 1` - View specific syscall stats
- `testprocinfo` - Display process scheduling info
- `dummyproc 20` - Create test process with 20 tickets