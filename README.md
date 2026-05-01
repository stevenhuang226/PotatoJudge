# PotatoJudge

## Introduction

Simple Linux-based judge backend.

---

## Global Config

Path:

```
#define CONF_PATH <path>
```

Fields:

```
base_submission=<path>
base_problem=<path>
base_workspace=<path>
```

* `base_submission`
  Directory containing submissions

* `base_problem`
  Directory containing problems

* `base_workspace`
  Working directory (must be under `/tmp`)

---

## Problem Structure

```
<base_problem>/<problem_id>/
```

Files:

* `config.conf`
* `driver`
* `checker.out`
* `input0.bin`, `input1.bin`, ...
* `output0.bin`, `output1.bin`, ...

Additionally:

```
<base_problem>/set_limit
```

* `set_limit`
  Executable used to apply runtime limits (seccomp)

### config.conf

case_count=%d
limit_time_s=%d
limit_as_mb=%d
limit_stack_mb=%d
expect_max_result_mb=%d

* `case_count`
  Number of test cases

* `limit_time_s`
  CPU time limit (seconds)

* `limit_as_mb`
  Memory limit (MB)

* `limit_stack_mb`
  Stack limit (MB)

* `expect_max_result_mb`
  Shared memory size (MB)
  Must be >= max(input size, output size)

---

## Submission Structure

```
<base_submission>/<submission_id>/
```

Files:

* `solution`
* `detail.conf`

### solution

Must define:

```c
char *solution();
```

Example:

```c
char *solution()
{
    return "Hello World!";
}
```

---

### detail.conf

compiler_type=%d

* `1` → gcc
* `2` → g++ (reserved)

```
```

---

## Judge Flow

1. Read global config
2. Load problem + submission
3. Compile:

   ```
   set_limit + driver + solution → a.out
   ```
4. Apply limits:

   * CPU
   * memory
   * stack
   * seccomp (via `set_limit`)
5. Execute program
6. Use shared memory to collect output
7. Run `checker.out`
8. Write result:
   * `<base_submission>/<submission_id>/done.json`

---

## Notes

* No stdin/stdout is used
* Output is written via shared memory
* Max output size is limited by `expect_max_result_mb`
* Use `<base_problem>/set_limit` to block dangerous syscalls

---

