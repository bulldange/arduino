// Copyright Todd Treece
// Copyright 2017 David Conran

#include "IRrecv.h"
#include "IRsend.h"
#include "IRtimer.h"

//                       DDDD   IIIII   SSSS  H   H
//                        D  D    I    S      H   H
//                        D  D    I     SSS   HHHHH
//                        D  D    I        S  H   H
//                       DDDD   IIIII  SSSS   H   H

// DISH support originally by Todd Treece
//   http://unionbridge.org/design/ircommand

// Constants
// Ref:
//   https://github.com/marcosamarinho/IRremoteESP8266/blob/master/ir_Dish.cpp
//   http://www.hifi-remote.com/wiki/index.php?title=Dish
#define DISH_HDR_MARK    400U
#define DISH_HDR_SPACE  6100U
#define DISH_BIT_MARK    400U
#define DISH_ONE_SPACE  1700U
#define DISH_ZERO_SPACE 2800U
#define DISH_RPT_SPACE  DISH_HDR_SPACE

#if SEND_DISH
// Send an IR command to a DISH NETWORK device.
//
// Args:
//   data:   The contents of the command you want to send.
//   nbits:  The bit size of the command being sent.
//   repeat: The number of times you want the command to be repeated.
//
// Status: BETA / Previously working.
//
// Note:
//   Dishplayer is a different protocol.
//   Typically a DISH device needs to get a command a total of at least 4
//   times to accept it. e.g. repeat=3
//
//   Here is the LIRC file I found that seems to match the remote codes from the
//   oscilloscope:
//     DISH NETWORK (echostar 301):
//     http://lirc.sourceforge.net/remotes/echostar/301_501_3100_5100_58xx_59xx
//
// Ref:
//   http://www.hifi-remote.com/wiki/index.php?title=Dish
void IRsend::sendDISH(uint64_t data, uint16_t nbits, uint16_t repeat) {
  // Set 57.6kHz IR carrier frequency, duty cycle is unknown.
  enableIROut(57600);
  // Header
  mark(DISH_HDR_MARK);
  space(DISH_HDR_SPACE);
  // We always send a command, even for repeat=0, hence '<= repeat'.
  for (uint16_t i = 0; i <= repeat; i++) {
    // Data
    sendData(DISH_BIT_MARK, DISH_ONE_SPACE, DISH_BIT_MARK, DISH_ZERO_SPACE,
             data, nbits, true);
    // Footer
    mark(DISH_BIT_MARK);
    space(DISH_RPT_SPACE);
  }
}
#endif

#if DECODE_DISH
// Decode the supplied DISH NETWORK message.
//
// Args:
//   results: Ptr to the data to decode and where to store the decode result.
//   nbits:   Nr. of bits to expect in the data portion. Typically DISH_BITS.
//   strict:  Flag to indicate if we strictly adhere to the specification.
// Returns:
//   boolean: True if it can decode it, false if it can't.
//
// Status:  ALPHA (untested and unconfirmed.)
//
// Note:
//   Dishplayer is a different protocol.
//   Typically a DISH device needs to get a command a total of at least 4
//   times to accept it.
// Ref:
//   http://www.hifi-remote.com/wiki/index.php?title=Dish
//   http://lirc.sourceforge.net/remotes/echostar/301_501_3100_5100_58xx_59xx
//   https://github.com/marcosamarinho/IRremoteESP8266/blob/master/ir_Dish.cpp
bool IRrecv::decodeDISH(decode_results *results, uint16_t nbits, bool strict) {
  if (results->rawlen < 2 * nbits + HEADER + FOOTER - 1)
    return false;  // Not enough entries to be valid.
  if (strict && nbits != DISH_BITS)
    return false;  // Not strictly compliant.

  uint64_t data = 0;
  uint16_t offset = OFFSET_START;

  // Header
  if (!match(results->rawbuf[offset++], DISH_HDR_MARK))
    return false;
  if (!matchSpace(results->rawbuf[offset++], DISH_HDR_SPACE))
    return false;

  // Data
  uint16_t actual_bits;
  for (actual_bits = 0; offset < results->rawlen; actual_bits++, offset++) {
    if (!match(results->rawbuf[offset++], DISH_BIT_MARK))
      return false;
    if (matchSpace(results->rawbuf[offset], DISH_ONE_SPACE))
      data = (data << 1) | 1;  // 1
    else if (matchSpace(results->rawbuf[offset], DISH_ZERO_SPACE))
      data = data << 1;  // 0
    else
      break;
  }

  // Footer
  if (!match(results->rawbuf[offset - 1], DISH_BIT_MARK))
    return false;

  // Compliance
  if (strict) {
    // The DISH protocol calls for a repeated message, so strictly speaking
    // there should be a code following this. Only require it if we are set to
    // strict matching.
    if (!matchSpace(results->rawbuf[offset], DISH_RPT_SPACE))
      return false;
    if (actual_bits != nbits)
      return false;  // We didn't get the same number of bits we asked for.
  }

  // Success
  results->decode_type = DISH;
  results->bits = actual_bits;
  results->value = data;
  results->address = 0;
  results->command = 0;
  return true;
}
#endif
