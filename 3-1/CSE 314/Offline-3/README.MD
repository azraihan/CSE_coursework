# Intelligence Operation Simulation

This assignment simulates a **multi-threaded intelligence operation** using pthreads, demonstrating synchronization concepts including semaphores, mutexes, and reader-writer patterns.

## Scenario

Intelligence operatives must complete document recreation at typewriting stations, then group leaders distribute intelligence while staff monitor the logbook.

## Synchronization Features

- **Station Management**: 4 typewriting stations with semaphore-based access control
- **Reader-Writer Pattern**: Intelligence staff can read logbook concurrently, but group leaders need exclusive write access
- **Group Coordination**: Group leaders wait for all team members before proceeding
- **Thread-Safe Output**: Mutex-protected console output

## System Components

- **Operatives**: Complete document recreation, organized in groups
- **Group Leaders**: Coordinate team completion and update logbook
- **Intelligence Staff**: Monitor operation progress via logbook reading

## Files

- `2105009.cpp` - Main simulation program
- `in.txt` - Input parameters (N operatives, M group size, x recreation time, y logbook time)
- `out.txt` - Simulation output log

## Usage

### Compilation and Execution
```bash
g++ -pthread 2105009.cpp
./a.out in.txt out.txt
```

### Input Format
```
N M
x y
```
- **N**: Number of operatives
- **M**: Group size (operatives per unit)
- **x**: Maximum recreation time
- **y**: Logbook writing time

### Example
```
15 5
10 3
```
- 15 operatives in groups of 5
- Recreation time: 1-10 units
- Logbook writing: 3 units

## Output

The simulation logs all events with timestamps, showing:
- Operative arrivals and station assignments
- Document recreation progress
- Group completion notifications
- Intelligence staff logbook access
- Final operation summary