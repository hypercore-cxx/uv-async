async.h
=======

Asynchronous goodies built on libuv

# install

```c
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

# license

MIT
