## a problem have:
driver.c: compile with judge code. Call judge function and write judge's result into shared memory
input.bin: sandbox(parent) will load this into shared memory (void \*). Then driver should know how to read this
output.bin: used for checker. Not force request. sandbox(parent) will load this into a memory space (void \*)
check.c: sandbox(parent) will call this to check result;


## Judeg Process:
sandbox(parent): Compile driver.c and judge.c as judge.out
load input.bin into shared\_memory.
fork and run judge.out

load output.bin if exists.
call check to check result, result will depend on this.
