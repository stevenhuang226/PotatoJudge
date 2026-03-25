## a problem have:
config              plain text file include settings for this test case
drive.c             compile with judge.c. Call judge function and write judge's result into shared memory
checker.out/c       call by parent, check result in shared memory. Can use data in output.bin to assists

n x input.bin:      load by parent
n x output.bin:     load by parent, can be use by checker

## Sandbox

judge.c

comiple judge.c and driver.c in sandbox environment, generate judge.out -> check CE

n x loop{
    load input.bin -> shared memory

    create sandbox {
        run judge.out
    }

    load output.bin if exists
    call checker
}
