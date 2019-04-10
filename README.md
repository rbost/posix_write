# POSIX Write test

A snippet showing issues with the POSIX `write` call.

On Mac OS (and thus probably BSD), calling `write` with a byte count larger than 2^31 returns an `EINVAL` error.
This is not the case with Linux (tested on Ubuntu 14 LTS and Ubuntu 18 LTS).

This is an issue as, according to the BSD System Calls Manual (*cf.* `man 2 write` on Mac OS), `write` fails with `EINVAL` when "`fildes` [the file descriptor] is not a valid file descriptor open for writing": the observed behavior is undocumented.

The result of this experiment can be seen [on Travis](<https://travis-ci.org/rbost/posix_write>).