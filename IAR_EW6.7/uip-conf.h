#include "types.h"

/* For now, take the defaults */
typedef uint16_t u16_t;
typedef uint8_t u8_t;

typedef u16_t uip_stats_t;

/* We simply don't have the RAM to waste on extra connections */
#define UIP_CONF_MAX_CONNECTIONS 2

/* We don't have room for an entire ethernet frame .. rats! */
#define UIP_CONF_BUFSIZE     500

/* Already defined a buffer. Should use that. */
#define UIP_CONF_EXTERNAL_BUFFER 1
extern u8_t *uip_buf;

typedef u16_t uip_tcp_appstate_t;
#define UIP_APPCALL uip_callback
void uip_callback(void);

