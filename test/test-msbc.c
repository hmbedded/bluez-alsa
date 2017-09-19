/*
 * test-msbc.c
 * Copyright (c) 2016-2017 Arkadiusz Bokowy
 *
 * This file is a part of bluez-alsa.
 *
 * This project is licensed under the terms of the MIT license.
 *
 */

#include "inc/sine.inc"
#include "inc/test.inc"
#include "../src/msbc.c"
#include "../src/shared/ffb.c"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

int test_msbc_find_h2_header(void) {

	static const uint8_t raw[][10] = {
		{ 0 },
		/* H2 header starts at first byte */
		{ 0x01, 0x08, 0xad, 0x00, 0x00, 0xd5, 0x10, 0x00, 0x11, 0x10 },
		/* H2 header starts at 5th byte */
		{ 0x00, 0xd5, 0x10, 0x00, 0x01, 0x38, 0xad, 0x00, 0x11, 0x10 },
		/* first H2 header starts at 2nd byte (second at 6th byte) */
		{ 0xd5, 0x01, 0xc8, 0xad, 0x00, 0x01, 0xf8, 0xad, 0x11, 0x10 },
		/* incorrect sequence number (bit not duplicated) */
		{ 0x01, 0x18, 0xad, 0x00, 0x00, 0xd5, 0x10, 0x00, 0x11, 0x10 },
		{ 0x01, 0x58, 0xad, 0x00, 0x00, 0xd5, 0x10, 0x00, 0x11, 0x10 },
	};

	size_t len;

	len = sizeof(*raw);
	assert(msbc_find_h2_header(raw[0], &len) == NULL);
	assert(len == 1);

	len = sizeof(*raw);
	assert(msbc_find_h2_header(raw[1], &len) == (esco_h2_header_t *)&raw[1][0]);
	assert(len == sizeof(*raw) - 0);

	len = sizeof(*raw);
	assert(msbc_find_h2_header(raw[2], &len) == (esco_h2_header_t *)&raw[2][4]);
	assert(len == sizeof(*raw) - 4);

	len = sizeof(*raw);
	assert(msbc_find_h2_header(raw[3], &len) == (esco_h2_header_t *)&raw[3][1]);
	assert(len == sizeof(*raw) - 1);

	len = sizeof(*raw);
	assert(msbc_find_h2_header(raw[4], &len) == NULL);
	assert(len == 1);

	len = sizeof(*raw);
	assert(msbc_find_h2_header(raw[5], &len) == NULL);
	assert(len == 1);

	return 0;
}

int test_msbc_encode_decode(void) {

	struct esco_msbc msbc = { .init = false };
	uint8_t sine[1024 * 2];
	size_t len;
	size_t i;

	assert(msbc_init(&msbc) == 0);
	snd_pcm_sine_s16le((int16_t *)sine, sizeof(sine) / sizeof(int16_t), 1, 0, 0.01);

	uint8_t data[sizeof(sine)];
	uint8_t *data_tail = data;

	for (i = 0; i < sizeof(sine); ) {

		len = ffb_len_in(&msbc.enc_pcm);
		len = MIN(sizeof(sine) - i, len);
		memcpy(msbc.enc_pcm.tail, &sine[i], len);
		ffb_seek(&msbc.enc_pcm, len);
		i += len;

		msbc_encode(&msbc);

		len = ffb_len_out(&msbc.enc_data);
		memcpy(data_tail, msbc.enc_data.data, len);
		ffb_rewind(&msbc.enc_data, len);
		data_tail += len;

	}

	assert((data_tail - data) == 480);

	uint8_t pcm[sizeof(sine)];
	uint8_t *pcm_tail = pcm;

	for (i = 0; i < (size_t)(data_tail - data); ) {

		len = ffb_len_in(&msbc.dec_data);
		len = MIN((data_tail - data) - i, len);
		memcpy(msbc.dec_data.tail, &data[i], len);
		ffb_seek(&msbc.dec_data, len);
		i += len;

		msbc_decode(&msbc);

		len = ffb_len_out(&msbc.dec_pcm);
		memcpy(pcm_tail, msbc.dec_pcm.data, len);
		ffb_rewind(&msbc.dec_pcm, len);
		pcm_tail += len;

	}

	assert((pcm_tail - pcm) == 1920);

	msbc_finish(&msbc);
	return 0;
}

int main(void) {
	test_run(test_msbc_find_h2_header);
	test_run(test_msbc_encode_decode);
	return 0;
}
