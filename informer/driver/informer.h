#define IA32_THERM_STATUS_MSR 0x19C	// CPU reg to read the temperature (bits 22:16

#define TJ_MAX	100	// max junction temperature in Celsius

// function prototypes
void test_temp_read(void);