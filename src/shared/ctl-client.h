/*
 * BlueALSA - ctl-client.h
 * Copyright (c) 2016-2017 Arkadiusz Bokowy
 *
 * This file is a part of bluez-alsa.
 *
 * This project is licensed under the terms of the MIT license.
 *
 */

#ifndef BLUEALSA_SHARED_CTLCLIENT_H_
#define BLUEALSA_SHARED_CTLCLIENT_H_

#include <stdbool.h>
#include "shared/ctl-proto.h"

int bluealsa_open(const char *interface);

int bluealsa_subscribe(int fd, enum event mask);

ssize_t bluealsa_get_devices(int fd, struct msg_device **devices);
ssize_t bluealsa_get_transports(int fd, struct msg_transport **transports);

struct msg_transport *bluealsa_get_transport(int fd, bdaddr_t addr,
		enum pcm_type type, enum pcm_stream stream);

int bluealsa_get_transport_delay(int fd, const struct msg_transport *transport);

int bluealsa_open_transport(int fd, const struct msg_transport *transport);
int bluealsa_close_transport(int fd, const struct msg_transport *transport);
int bluealsa_pause_transport(int fd, const struct msg_transport *transport, bool pause);
int bluealsa_drain_transport(int fd, const struct msg_transport *transport);
int bluealsa_set_transport_volume(int fd, const struct msg_transport *transport,
									uint8_t ch1_muted, uint8_t ch2_muted,
									uint8_t ch1_volume, uint8_t ch2_volume);
#endif
