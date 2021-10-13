#pragma once

// input configuration: bipolar/unipolar, single ended or differential
#define INCC_BIPOLAR_DIFF (0b000) // 00X
#define INCC_BIPOLAR_COM (0b010)
#define INCC_TEMP (0b011)
#define INCC_UNIPOLAR_DIFF (0b100) // 10X
#define INCC_UNIPOLAR_REF_COM (0b110)
#define INCC_UNIPOLAR_REF_GND (0b111)

// reference voltage (note: decoupling caps required on REF/REFIN when using INT_REF!)
#define INT_REF_25 (0b000)
#define INT_REF_4096 (0b001)
#define EXT_REF_TEMP_ON (0b010)
#define EXT_REF_TEMP_BUF (0b011)
#define EXT_REF_TEMP_OFF (0b110)
#define EXT_REF_TEMP_OFF_BUF (0b111)

// sequencer configuration (default: no sequencer)
#define SEQ_OFF (0b00)
#define SEQ_UPDATE (0b01)
#define SEQ_SCAN_INPUT_TEMP (0b10)
#define SEQ_SCAN_INPUT (0b11)

#define MAX_FREQ (38000000) // 26 ns period @ VDD 5V and VIO 3.3 - 5V

#define UNIPOLAR_MODE (0)
#define BIPOLAR_MODE (1)
#define DIFFERENTIAL_MODE (2)

#define REF_INTERNAL (0)
#define REF_EXTERNAL (1)
#define REF_GND (2)
#define REF_COM (3)

// bit shifts needed for config register values, from datasheet p. 27 table 11:
#define CFG (13)
#define INCC (10)
#define INx (7)
#define BW (6)
#define REF (3)
#define SEQ (1)
#define RB (0)
