
#ifndef __ASYNC_H__
#define __ASYNC_H__ 1

#include <stdlib.h>
#include <unistd.h>
#include <uv.h>

#define async(env, l)                                      \
  async_env_t *env = malloc(sizeof(async_env_t));          \
  env->loop = l;                                           \
  env->handle = NULL;

#define async_work_init(e, fn, d, f) {                     \
  async_work_data_t data;                                  \
  uv_work_t work;                                          \
  data.env = e;                                            \
  data.cb = fn;                                            \
  data.data = (void *)d;                                   \
  data.flags = 0;                                          \
  data.flags |= f;                                         \
  work.data = (void *) &data;                              \
  e->handle = (uv_async_t *) malloc(sizeof(uv_async_t));   \
  uv_async_init(e->loop, e->handle,                        \
      _handle_async_callback);                             \
  uv_queue_work(e->loop, &work,                            \
      _handle_async,                                       \
      _handle_after_async);                                \
}

#define queue(env, fn) {                                   \
  async_work_init(env, fn, 1, ASYNC_DEFER);                \
}

#define wait(env, ms, fn) {                                \
  async_work_init(env, fn, ms, ASYNC_DEFER);               \
}

#define MAX_ASYNC_STDIO 8

// FLAGS

#define ASYNC_DEFER 0x1

struct async_work_data;

typedef struct async_env {
  uv_async_t *handle;
  uv_loop_t *loop;
  uv_stdio_container_t stdio[MAX_ASYNC_STDIO];
  int flags;
} async_env_t;

typedef struct async_work_data {
  async_env_t *env;
  void (*cb)(struct async_work_data *data);
  void *data;
  int flags;
  int err;
} async_work_data_t;

typedef void (async_cb)(async_work_data_t *work);

void
aspawn (async_env_t *env, char **args, async_cb *fn);

static void
_handle_async_callback (uv_async_t *handle, int rc);

static void
_handle_async (uv_work_t *work);

static void
_handle_after_async (uv_work_t *work, int rc);

static void
_spawn_async (async_env_t *env, int argc, char *args[], void (*fn)(async_work_data_t *work));

static void
_handle_spawn_async (uv_process_t *req, int64_t rc, int sig);


void
aspawn (async_env_t *env, char *args[], void (*fn)(async_work_data_t *work)) {
  int argc = -1;
  char *a = args[++argc];
  while (1) {
    a = args[argc +1];
    if (a != NULL) {
      argc++;
    } else {
      break;
    }
  }

  _spawn_async(env, argc, args, fn);
}

static void
_handle_async (uv_work_t *work) {
  async_work_data_t *data = (async_work_data_t *)work->data;
  async_env_t *env = (async_env_t *)data->env;
  int us = 0;

  if (ASYNC_DEFER == (ASYNC_DEFER & data->flags)) {
    us = ((int) data->data) * 1000;
  }

  usleep(us);
  data->cb(data);
}

static void
_handle_after_async (uv_work_t *work, int rc) {
  async_work_data_t *data = (async_work_data_t *)work->data;
  async_env_t *env = (async_env_t *)data->env;
  uv_async_t *handle = env->handle;
  handle->data = data;

  uv_async_send(handle);
}

static void
_handle_async_callback (uv_async_t *handle, int rc) {
  uv_work_t *work = (uv_work_t *) handle->data;
  async_work_data_t *data = (async_work_data_t *)work->data;
  async_env_t *env = (async_env_t *)data->env;

  if (0 == uv_is_closing((uv_handle_t *)handle)) {
    uv_close((uv_handle_t *)handle, NULL);
  }
}

static void
_spawn_async (async_env_t *env, int argc, char *args[], void (*fn)(async_work_data_t *work)) {
  int i = 0;
  int n = 0;
  uv_process_options_t opts;
  uv_process_t *process = malloc(sizeof(uv_process_t));
  async_work_data_t *data = malloc(sizeof(async_work_data_t));

  args[argc] = NULL;
  opts.exit_cb = _handle_spawn_async;
  opts.args = args;
  opts.file = args[0];
  opts.flags = 0;

  if (NULL != env->stdio) {
    opts.stdio = env->stdio;

    for (; i < MAX_ASYNC_STDIO; ++i) {
      if (NULL != env->stdio[i].data.stream) n++;
    }

    opts.stdio_count = n;
    printf("%d\n", n);
  }

  data->env = env;
  data->cb = fn;
  process->data = (void *) data;

  if (0 != uv_spawn(env->loop, process, &opts)) {
    data->err = 1;
    fn(data);
    free(data);
    free(process);
  }
}


static void
_handle_spawn_async (uv_process_t *process, int64_t rc, int sig) {
  async_work_data_t *work = (async_work_data_t *) process->data;
  async_cb *cb = (async_cb *) work->cb;
  if (NULL != work->cb) {
    cb(work);
  }

  free(process);
  free(work);
  printf("done\n");
}

#endif
