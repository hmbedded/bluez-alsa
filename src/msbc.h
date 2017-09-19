/*
 * BlueALSA - msbc.h
 * Copyright (c) 2016-2017 Arkadiusz Bokowy
 *               2017 Juha Kuikka
 *
 * This file is a part of bluez-alsa.
 *
 * This project is licensed under the terms of the MIT license.
 *
 */

#ifndef BLUEALSA_MSBC_H_
#define BLUEALSA_MSBC_H_

#include <stdbool.h>
#include <sbc/sbc.h>
#include "shared/ffb.h"

/* HFP uses SBC encoding with precisely defined parameters. Hence, the size
 * of the input (number of PCM samples) and output is known up front. */
#define MSBC_CODESIZE 240
#define MSBC_FRAMELEN 57

#define ESCO_H2_SYNCWORD   0x801

/**
 * Synchronization header for eSCO transparent data. */
typedef struct esco_h2_header {
	uint16_t sync:12;
	uint16_t sn0:2;
	uint16_t sn1:2;
} __attribute__ ((packed)) esco_h2_header_t;

typedef struct esco_msbc_frame {
	esco_h2_header_t header;
	uint8_t payload[MSBC_FRAMELEN];
	uint8_t padding;
} __attribute__ ((packed)) esco_msbc_frame_t;

struct esco_msbc {

	/* decoder */
	sbc_t dec_sbc;
	/* encoder */
	sbc_t enc_sbc;

	/* buffer for incoming eSCO frames */
	struct ffb dec_data;
	/* buffer for outgoing PCM samples */
	struct ffb dec_pcm;

	/* buffer for incoming PCM samples */
	struct ffb enc_pcm;
	/* buffer for outgoing eSCO frames */
	struct ffb enc_data;

	size_t enc_frames;

	/* Determine whether structure has been initialized. This field is
	 * used for reinitialization - it makes msbc_init() idempotent. */
	bool init;

};

int msbc_init(struct esco_msbc *msbc);
void msbc_finish(struct esco_msbc *msbc);

void msbc_decode(struct esco_msbc *msbc);
void msbc_encode(struct esco_msbc *msbc);

#endif
