uv-async
=======

Asynchronous goodies built on libuv

# install

```sh
$ clib install jwerle/async.h
```

# usage

Starting async work can be done with the `async(env, loop)` macro
function.

```c

int
main (void) {
	uv_loop_t *loop = uv_default_loop();

	async(env, loop) {
		queue(env, job);
		wait(env, 500, called_500ms_later);
		interval(env, 300, every_300ms);
	}

	return uv_run(loop, UV_RUN_DEFAULT);
}
```

## queue

You can queue a task with the `queue(env, fn)` macro function

```c
async(env, loop) {
	queue(env, job_fn);
}
```

## timeout

You can set a task after a timeout with the `wait(env, ms, fn)` macro
function.

```c
async(env, loop) {
	wait(env, 500, job_fn);
}
```

## interval

You can set an interval task with the `inteval(env, ms, fn)` macro
function.

```c
async(env, loop) {
	interval(env, 500, job_fn);
}
```

You can stop the inteval by setting the `rc` to `1` on the
`async_work_data_t *data` pointer passed to the function.

```c
static void
on_interval (async_work_data_t  *work) {
	if (++interval_count > 10) {
		printf("interval limit reached. Stopping.. \n");
		data->rc = 1;
	} else {
		printf("interval #%d\n", interval_count);
	}
}
```

## spawn

You can spawn a system command with the `spawn(env, cmd, fn)` macro. The
`fn` will be called when the command has returned. The return code and
signal code are attached to the `async_work_data_t *` pointer provided
to the callback `fn`.

```c
#include "async.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

staic void
on_spawn (async_work_data_t *work) {
	assert(0 == work->rc);
	assert(0 == work->signal);
	assert(work->process);

	struct stat s;
	assert(0 == stat("./foo", &s));
	assert(0 == stat("./foo/bar", &s));
	assert(0 == stat("./foo/bar/biz", &s));
}

int
main (void) {
	async(env, uv_default_loop()) {
		char *cmd[] = { "mkdir", "-p", "./foo/bar/biz" };
		spawn(env, cmd, on_spawn);
	}

	return uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
```

# license

MIT
