# PotatoJudge
## introduction
- A simple judge backend.
- Running on Linux
## /etc/potatojudge.conf
```base_submission=``` path to where submission file. The user file should put under this director\
```base_problem=``` path to problem files. The problem input/output/config will be here\
```base_worksapace=``` !! this should put under /tmp !! It will create some trash file. And judge will running under this.

## base_problem/X(id)/config.conf
```case_count=1``` test case's count\
```limit_time_s=2``` cpu run time limit(second)\
```limit_as_mb=8``` memory limit(mb)\
```limit_stack_mb=8``` stack limit(mb)\
```expect_max_result_mb=8``` file size limit (mb)

## base_submission/X(id)/detail.conf
```compiler_type=1```\
- 1 gcc\
- 2 g++\
use this to set compiler type (only gcc is supported currently)
