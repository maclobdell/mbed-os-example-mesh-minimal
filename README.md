# Example Mesh application for mbed OS

This applications tries to be the simplest one that utilizes our Mesh networking stack.

Basically only thing it does, is to join unsecure *6LoWPAN-ND* network.

See file `mbed_app.json` for example how to define IEEE 802.15.4 channel to use.

## Changing the radio driver

In order to run 6LoWPAN-ND network, you need working RF driver for Nanostack.

Provided example uses Atmel AT86RF233. To replace that do:

```
mbed remove atmel-rf-driver
```

and then

```
mbed add <your RF driver address>
```

And then just `mbed compile`


## Lowering the RAM+Flash usage

**WARNING:** These are experimental and unsupported tests to get this demo to running on low Flash/RAM devices. Do not in any circumstace use these to base your application to.

### Remove frameworks.
Delete everything under mbed-os/frameworks

```
rm -rf mbed-os/frameworks/*
```

These are used only by testing utilities

## Turn off all but 6LOWPAN-ND stack

For this demo, we do not need Thread stack.
Open `mbed-os/features/FEATURE_IPv6/sal-stack-nanostack-private/source/configs/mbedOS.cfg`

Modify the content to be

```
#include "base/lowpan_host.cfg.cfg"
```

This will turn of all other but 6LoWPAN-ND based non-routing host support.

## Disable router functionality

Next, we enable just the 6LoWPAN-ND HOST functionality and disable the routing completely.

From file `mbed-os/features/FEATURE_IPV6/mbed-mesh/api/source/nd_tasklet.c` modify line 373 to:

```
tasklet_data_ptr->mode = NET_6LOWPAN_HOST;
```

## Lower the Nanostack's internal heap size

Modify your `mbed_app.json` and add value `"mbed-mesh-api.heap-size": 8192` under `target_overrides`

8kB is enough for 6LoWPAN-ND host.

## Get rid of mbed-client dependencies.

*NOTE:* Expect these changes to be integrated into core os. This is purely experimental for now.

Edit `mbed-os/features/FEATURE_IPV6/sal-stack-nanostack-private/source/6LoWPAN/thread/thread_test_api.c` and exclude all `#include` directives after `net_thread_test.h` like this

```
#include <net_thread_test.h>

#ifdef HAVE_THREAD
...includes here...
#endif
```

Now move `mbed-client-randlib` from `mbed-os/features/FEATURE_CLIENT/` to under `mbed-os/features/FEATURE_IPV6`

This allows you to completely drop the `FEATURE_CLIENT` so modify `mbed_app.json` to have

```
"target.features_add": ["IPV6"]
```

## Turn off all but AES security features from mbedTLS

Open up the `mbed-os/features/mbedtls/inc/mbedtls/config.h`

Remove everything and replace with only:

```
#define MBEDTLS_AES_C
#define MBEDTLS_AES_ROM_TABLES
```

## Remove extra stack space from Nanostacks event-loop

Edit file `mbed-os/features/FEATURE_IPV6/nanostack-hal-mbed-cmsis-rtos/ns_event_loop.c`

Change line 19 to

```
static osThreadDef(event_loop_thread, osPriorityNormal, /*1,*/ 1024);
```

## Lower the stack of timer thread

This is unstable feature but appears to work. So we lower the stack of Nanostack's timer thread to 1kB.

From file `mbed-os/features/FEATURE_IPV6/nanostack-hal-mbed-cmsis-rtos/arm_hal_timer.cpp` modify line 36 to:

```
static osThreadDef(timer_thread, osPriorityRealtime, /*1,*/ 1024);
```


