This demo exhibits the race condition occuring when threads simultaneously modify the same memory location.

This is because the access to this variable is not atomic and the context switch can occur in between, so
and old value of the variable will be incremented and will overwrite the incremented value, thus holding
it back.

read counter
add 1
write counter

Then compile it with one of two options:
#define USE_MUTEX
or
#define USE_ATOMIC

The code will run correctly with both of them, but the performance with atomic will be much better
