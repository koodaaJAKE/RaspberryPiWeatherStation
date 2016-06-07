/*
 * SerializeDeserialize.c
 *
 * This library is for serializing a binary data (integer and float) to send them through
 * network interface.
 */
#include "SerializeDeserialize.h"

unsigned char *serializeInt(unsigned char *buffer, int value)
{
  /* Write big-endian int value into buffer; assumes 32-bit int and 8-bit char. */
  buffer[0] = value >> 24;
  buffer[1] = value >> 16;
  buffer[2] = value >> 8;
  buffer[3] = value;

  return buffer + 4;
}

unsigned char *serializeFloat(unsigned char *buffer, float FloatValue)
{
	int value;

	value = Serialize754_32(FloatValue);

    buffer[0] = value >> 24;
    buffer[1] = value >> 16;
    buffer[2] = value >> 8;
    buffer[3] = value;

    return buffer + 4;
}

unsigned int Serialize754Float(float f, unsigned int bits, unsigned int expbits)
{
    float fnorm;
    int shift;
    int sign, exp, significand;
    unsigned int significandbits = bits - expbits - 1; // -1 for sign bit

    if (f == 0.0) return 0;

    // check sign and begin normalization
    if (f < 0) {
    	sign = 1;
    	fnorm = -f;
    }
    else {
    	sign = 0;
    	fnorm = f;
    }

    // get the normalized form of f and track the exponent
    shift = 0;
    while(fnorm >= 2.0) {
    	fnorm /= 2.0;
    	shift++;
    }
    while(fnorm < 1.0) {
    	fnorm *= 2.0;
    	shift--;
    }
    fnorm = fnorm - 1.0;

    // calculate the binary form (non-float) of the significant data
    significand = fnorm * ((1<<significandbits) + 0.5f);

    // get the biased exponent
    exp = shift + ((1<<(expbits-1)) - 1); // shift + bias

    // return the final answer
    return (sign<<(bits-1)) | (exp<<(bits-expbits-1)) | significand;
}

float Deserialize754Float(unsigned int f, unsigned int bits, unsigned int expbits)
{
	float result;
	int shift;
	unsigned int bias;
	unsigned int significandbits = bits - expbits - 1; // -1 for sign bit

	if (f == 0) return 0.0;

    // pull the significant
	result = (f&((1 <<significandbits)-1)); // mask
	result /= (1 <<significandbits); // convert back to float
	result += 1.0f; // add the one back on

	// deal with the exponent
	bias = (1<<(expbits-1)) - 1;
	shift = ((f>>significandbits)&((1 <<expbits)-1)) - bias;
	while(shift > 0) {
		result *= 2.0;
		shift--;
	}
	while(shift < 0) {
		result /= 2.0;
		shift++;
	}

	// sign it
	result *= (f>>(bits-1))&1? -1.0: 1.0;

	return result;
}

unsigned char *serializeStruct(unsigned char *buffer, const thread_data_t *Data)
{
	buffer = serializeFloat(buffer, Data->MPL3115A2temperature);
	buffer = serializeFloat(buffer, Data->humidity);
	return buffer;
}

