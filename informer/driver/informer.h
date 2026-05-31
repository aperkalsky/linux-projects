#define IA32_THERM_STATUS_MSR 0x19C	// CPU reg to read the temperature delta (bits 22:16), reported as degrees below TjMax

#define TJ_MAX	100	// max junction temperature in Celsius

// function prototypes
void test_temp_read(void);