## Usage
Run the script `organize.sh` with the following arguments:
```bash
cd Workspace
./organize.sh <submissions_folder> <target_folder> <tests_folder> <answers_folder> [OPTIONS]
```

## Optional Arguments
- `-v`: Verbose mode (prints execution details).
- `-noexecute`: Skip Task C (no execution or matching).
- `-nolc`: Skip line count calculation.
- `-nocc`: Skip comment count calculation.
- `-nofc`: Skip function count calculation (bonus task).

## Examples
```bash
./organize.sh submissions targets tests answers -v
./organize.sh submissions targets tests answers -v -noexecute
./organize.sh submissions targets tests answers -v -nolc -nocc
```