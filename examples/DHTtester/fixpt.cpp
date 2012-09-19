/**
 * Copyright (C) 2012 Chris Andreae
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "fixpt.h"

#include <stdlib.h>

template <class fix_t>
size_t fixpt_str(fix_t fpnum, uint8_t frac_bits, char *buf, size_t buflen, uint8_t precision) {
	// handle impossible buffer
	if(buflen < 2){ return 0; }

	fix_t frac_mask = (1 << frac_bits) - 1;

	int     strlen = 0;
	uint8_t dp     = 0;
	fix_t   integral   = fpnum >> frac_bits;
	fix_t   fractional = fpnum  &  frac_mask;

	// handle negative
	if(fpnum < 0){
		*buf++ = '-';
		--buflen;
		// and make positive
		integral |= ~(~0U >> frac_bits);
		integral = -integral;

		// if any 2^-ns are subtracted
		if(fractional){
			// decrement the integral: the fraction is taking away from it rather than adding
			integral--;
			// and it needs to be sign-extended and negated
			fractional |= ~frac_mask;
			fractional = -fractional;
		}
	}

	// integral part: figure magnitude and read digits in reverse
	uint8_t log10 = 0;
	for(fix_t i = integral; i != 0; i /= 10){ ++log10; }

	do{
		buf[log10 - ++strlen] = '0' + (integral % 10);
		integral /= 10;
		if(strlen == buflen) goto overflow;
	}
	while(integral != 0);

	// fractional part
	buf[strlen++] = '.';
	if(strlen == buflen) goto overflow;

	// fractional is measured in 256ths. We want to calculate that as a
	// sum of x * 10^-y by repeatedly multiplying by 10 and divmod 256.
	// eg: 42/256 = (1 * 0.1 + 164/256 * 0.1)
	//            = (1 * 0.1 + 6 * 0.01 + 104/256 * 0.01)
	//            = (1 * 0.1 + 6 * 0.01 + 4 * 0.001 + 16/256 * 0.001) etc etc
	do {
		fractional *= 10;
		buf[strlen++] = '0' + (fractional >> frac_bits);
		fractional &= frac_mask;
		dp++;
		if(dp == precision){ // round and break
			// check next digit
			if(((fractional * 10) >> frac_bits) >= 5){
				// and round up as far as necessary
				uint8_t idx = strlen - 1;
				while(idx > 0){
					if(buf[idx] == '.') --idx;
					else if(buf[idx] == '9'){
						buf[idx] = '0';
						--idx;
					}
					else{
						buf[idx]++;
						break;
					}
				}
			}
			break;
		}
		if(strlen == buflen) goto overflow;
	} while (fractional != 0);

	// fill out the rest of the requested precision if needed
	while(dp++ < precision){
		buf[strlen++] = '0';
		if(strlen == buflen) goto overflow;
	}

	buf[strlen] = '\0';
	return strlen;

 overflow:
	buf[buflen - 1] = '\0';
	return buflen;
}

// instantiate all we're likely to use, let the linker sort out what we didn't
template size_t fixpt_str<uint16_t>(uint16_t fpnum, uint8_t frac_bits, char *buf, size_t buflen, uint8_t precision);
template size_t fixpt_str<int16_t>(int16_t fpnum, uint8_t frac_bits, char *buf, size_t buflen, uint8_t precision);
template size_t fixpt_str<uint32_t>(uint32_t fpnum, uint8_t frac_bits, char *buf, size_t buflen, uint8_t precision);
template size_t fixpt_str<int32_t>(int32_t fpnum, uint8_t frac_bits, char *buf, size_t buflen, uint8_t precision);
