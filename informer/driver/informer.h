#define IA32_THERM_STATUS 		0x19C	// CPU reg to read the temperature delta (bits 22:16), reported as degrees below TjMax
#define IA32_THERM_STATUS_MASK 	0x7F

#define TJ_MAX	100	// max junction temperature in Celsius

// function prototypes
int read_cpu_temperature(void);

static ssize_t informer_read(struct file *file,
                             char __user *buf,
                             size_t count,
                             loff_t *ppos);
