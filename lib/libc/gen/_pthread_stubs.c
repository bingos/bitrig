/*
 * Copyright (c) 2012 Marco Peereboom <marco@peereboom.us>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <pthread.h>

int		pthread_detach(pthread_t) __attribute__((weak));
int		pthread_equal(pthread_t, pthread_t) __attribute__((weak));
void		*pthread_getspecific(pthread_key_t) __attribute__((weak));
int		pthread_join(pthread_t, void **) __attribute__((weak));
pthread_t	pthread_self(void) __attribute__((weak));

int		pthread_key_create(pthread_key_t *,
		    void (*) (void *)) __attribute__((weak));
int		pthread_key_delete(pthread_key_t) __attribute__((weak));

int		pthread_mutexattr_destroy(pthread_mutexattr_t *)
		    __attribute__((weak));
int		pthread_mutexattr_init(pthread_mutexattr_t *)
		    __attribute__((weak));
int		pthread_mutexattr_settype(pthread_mutexattr_t *, int)
		    __attribute__((weak));

int		pthread_mutex_destroy(pthread_mutex_t *) __attribute__((weak));
int		pthread_mutex_init(pthread_mutex_t *,
		    const pthread_mutexattr_t *) __attribute__((weak));
int		pthread_mutex_lock(pthread_mutex_t *) __attribute__((weak));
int		pthread_mutex_trylock(pthread_mutex_t *) __attribute__((weak));
int		pthread_mutex_unlock(pthread_mutex_t *) __attribute__((weak));

int		pthread_cond_broadcast(pthread_cond_t *) __attribute__((weak));
int		pthread_cond_destroy(pthread_cond_t *) __attribute__((weak));
int		pthread_cond_signal(pthread_cond_t *) __attribute__((weak));
int		pthread_cond_timedwait(pthread_cond_t *, pthread_mutex_t *,
		    const struct timespec *) __attribute__((weak));
int		pthread_cond_wait(pthread_cond_t *, pthread_mutex_t *)
		    __attribute__((weak));

#include <errno.h>
/*ARGSUSED*/
int
pthread_detach(pthread_t p)
{
	return (EINVAL);
}

int
pthread_equal(pthread_t p1, pthread_t p2)
{
	return (EINVAL);
}

void *
pthread_getspecific(pthread_key_t key)
{
	return (NULL);
}

int
pthread_join(pthread_t p, void **b)
{
	return (EINVAL);
}

pthread_t
pthread_self(void)
{
	return (NULL);
}

int
pthread_key_create(pthread_key_t *key, void (*p) (void *b))
{
	return (ENOMEM);
}

int
pthread_key_delete(pthread_key_t key)
{
	return (EINVAL);
}

int
pthread_mutexattr_destroy(pthread_mutexattr_t *a)
{
	return (EINVAL);
}

int
pthread_mutexattr_init(pthread_mutexattr_t *a)
{
	return (ENOMEM);
}

int
pthread_mutexattr_settype(pthread_mutexattr_t *a, int i)
{
	return (EINVAL);
}

int
pthread_mutex_destroy(pthread_mutex_t *m)
{
	return (EINVAL);
}

int
pthread_mutex_init(pthread_mutex_t *m, const pthread_mutexattr_t *a)
{
	return (ENOMEM);
}

int
pthread_mutex_lock(pthread_mutex_t *m)
{
	return (EINVAL);
}

int
pthread_mutex_trylock(pthread_mutex_t *m)
{
	return (EBUSY);
}

int
pthread_mutex_unlock(pthread_mutex_t *m)
{
	return (EINVAL);
}

int
pthread_cond_broadcast(pthread_cond_t *c)
{
	return (EINVAL);
}

int
pthread_cond_destroy(pthread_cond_t *c)
{
	return (EINVAL);
}

int
pthread_cond_signal(pthread_cond_t *c)
{
	return (EINVAL);
}

int
pthread_cond_timedwait(pthread_cond_t *c, pthread_mutex_t *m,
    const struct timespec *ts)
{
	return (EINVAL);
}

int
pthread_cond_wait(pthread_cond_t *c, pthread_mutex_t *m)
{
	return (EINVAL);
}
