// ELA Blue Coin stuff

#ifdef	CONFIG_BT_NIMBLE_ENABLED
#define	ELA
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/ble_uuid.h"

typedef struct bleenv_s bleenv_t;
struct bleenv_s
{
   bleenv_t *next;              // Linked list
   ble_addr_t addr;             // Address (includes type)
   uint8_t namelen;             // Device name length
   char name[32];               // Device name (null terminated)
   char mac[13];                // MAC (hex)
   char better[13];             // ID (MAC) of better device
   int8_t betterrssi;           // Better RSSI
   int8_t rssi;                 // RSSI
   uint32_t lastbetter;         // uptime when last better entry
   uint32_t last;               // uptime of last seen
   uint32_t lastreport;         // uptime of last reported
   int16_t temp;                // Temp C*100
   int16_t tempreport;          // Temp last reported
   uint16_t volt;               // Bat voltage mV
   uint16_t hum;                // Hum %*100
   int8_t bat;                  // Bat %
   int16_t targetmin;           // Faikin, target C*100
   int16_t targetmax;           // Faikin, target C*100
   uint16_t co2;
   uint8_t power:1;             // Faikin power
   uint8_t rad:1;               // Faikin radiator
   uint8_t mode:3;              // Faikin mode          Unspecified,Auto,Fan,Dry,Cool,Heat,Reserved,Faikin
   uint8_t fan:3;               // Faikin fan           Unspecified,Auto,1,2,3,4,5,Quiet
   uint8_t updated:1;           // found/missing/set updated (for user to clear when they have handled)
   uint8_t changed:1;		// changed somehow, e.g. name (for user to clean when the have handled)
   uint8_t found:1;
   uint8_t missing:1;
   uint8_t namefull:1;
   uint8_t faikinset:1;         // If fields set
   uint8_t targetminset:1;
   uint8_t targetmaxset:1;
   uint8_t tempset:1;
   uint8_t humset:1;
   uint8_t batset:1;
   uint8_t voltset:1;
   uint8_t co2set:1;
};
extern bleenv_t *bleenv;

const char *ble_addr_format (ble_addr_t * a);
bleenv_t *bleenv_find (ble_addr_t * a, int make);       // Find a device by address
int bleenv_gap_disc (struct ble_gap_event *event);      // Handle GAP disc event
void bleenv_expire (uint32_t missingtime);      // Expire (i.e. missing)
void bleenv_clean (void);       // Delete old entries

void bleenv_bthome1 (const char *name, float c, float rh, uint16_t co2, float lux);
void bleenv_bthome2 (const char *name, float c, float rh, uint16_t co2, float lux);
void bleenv_faikin (const char *name, float c, float targetmin, float targetmax, uint8_t power, uint8_t rad, uint8_t mode,
                    uint8_t fan);

void bleenv_run (void);         // Run BLE for ELA
#endif