// ELA BlueCoin stuff
// https://pvvx.github.io/ATC_MiThermometer/TelinkMiFlasher.html

#include "revk.h"
#ifdef CONFIG_BT_NIMBLE_ENABLED
#include "bleenv.h"
#include "math.h"

static const char TAG[] = "bleenv";
#define	MAX_ADV	31

bleenv_t *bleenv = NULL;
static volatile uint8_t active = 30;    // Next run needs to be active (number of runs / seconds)

bleenv_t *
bleenv_find (ble_addr_t * a, int make)
{                               // Find (create) a device record
   bleenv_t *d;
   for (d = bleenv; d; d = d->next)
      if (d->addr.type == a->type && !memcmp (d->addr.val, a->val, 6))
         break;
   if (!d && !make)
      return d;
   if (!d)
   {
      d = mallocspi (sizeof (*d));
      if (!d)
         return d;
      memset (d, 0, sizeof (*d));
      d->addr = *a;
      sprintf (d->mac, "%02X%02X%02X%02X%02X%02X", a->val[5], a->val[4], a->val[3], a->val[2], a->val[1], a->val[0]);
      strcpy (d->name, d->mac); // Default name
      d->namelen = 12;
      d->next = bleenv;
      d->missing = 1;
      d->updated = 1;
      d->changed = 1;
      bleenv = d;
      active = 30;              // Get more info if we can
   }
   d->last = uptime ();
   return d;
}

int
bleenv_gap_disc (struct ble_gap_event *event)
{
   const uint8_t *p = event->disc.data,
      *e = p + event->disc.length_data;
   if (e > p + MAX_ADV)
      return 0;                 // Silly
   bleenv_t *d = bleenv_find (&event->disc.addr, 0);
   //if (d) ESP_LOG_BUFFER_HEX_LEVEL(event->disc.event_type == BLE_HCI_ADV_RPT_EVTYPE_SCAN_RSP ? "Rsp" : "Adv", event->disc.data, event->disc.length_data,ESP_LOG_ERROR);
   // Check if a temp device
   const uint8_t *name = NULL;
   const uint8_t *temp_2_100 = NULL;    // Temp as two bytes * 0.01
   const uint8_t *temp_2_10 = NULL;     // Temp as two bytes * 0.1
   const uint8_t *temp_1 = NULL;        // Temp as one byte
   const uint8_t *temp_1_35 = NULL;     // Temp as one byte, *0.35
   const uint8_t *bat = NULL;
   const uint8_t *volt = NULL;
   const uint8_t *hum_2_100 = NULL;     // Humidity * 0.01
   const uint8_t *hum_1 = NULL; // Humidity * 1
   const uint8_t *env = NULL;
   uint16_t co2 = 0;
   uint16_t man = 0;
   while (p < e)
   {
      const uint8_t *n = p + *p + 1;
      if (n > e)
         break;
      // Type 08/09 are name
      // Type 16 is 16 bit UUID
      if (p[0] > 1 && (p[1] == 8 || p[1] == 9))
         name = p;
      else if (*p == 5 && p[1] == 0x16 && p[2] == 0x6E && p[3] == 0x2A)
         temp_2_100 = p + 4;    // Temp UUID 2A6E
      else if (*p == 4 && p[1] == 0x16 && p[2] == 0x0F && p[3] == 0x18)
         bat = p + 4;           // Bat UUID 180F
      else if (*p == 4 && p[1] == 0x16 && p[2] == 0x19 && p[3] == 0x2A)
         bat = p + 4;           // Bat UUID 2A16
      else if (*p == 18 && p[1] == 0x16 && p[2] == 0x1A && p[3] == 0x18)
         env = p;               // Env UUID 181A
      else if (*p >= 3 && p[1] == 0x16 && p[2] == 0x1C && p[3] == 0x18)
      {                         // Used for BT Home v1
         const uint8_t *d = p + 4;
         while (d < n)
         {                      // first byte is type(3) and len(5) where type is 0=uint, 1=int, 2=float, 3=string, 4=AC, next byte is meaning
            if (*d == 0x02 && d[1] == 0x01)
               bat = d + 2;
            else if (*d == 0x23 && d[1] == 0x02)
               temp_2_100 = d + 2;
            else if (*d == 0x03 && d[1] == 0x03)
               hum_2_100 = d + 2;
            else if (*d == 0x03 && d[1] == 0x0C)
               volt = d + 2;
            else if (*d == 0x12 && d[1] == 0x02)
               co2 = d[2] + (d[3] << 8);
            d += 1 + (*d & 0x1F);
         }
      } else if (*p >= 4 && p[1] == 0x16 && p[2] == 0xD2 && p[3] == 0xFC && (p[4] & 0xE0) == 0x40 && !(p[4] & 0x01))
      {                         // Used for BT Home v2 (unencrypted)
         // https://bthome.io/format/
         const uint8_t *d = p + 5;
         while (d < n)
         {                      // first byte is type(3) and len(5) where type is 0=uint, 1=int, 2=float, 3=string, 4=AC, next byte is meaning
            if (*d == 0 && d + 2 <= n)
            {                   // Packet ID?
               d += 2;
               continue;
            }
            if (*d == 0x01 && d + 2 <= n)
            {                   // Battery
               bat = d + 1;
               d += 2;
               continue;
            }
            if (*d == 0x02 && d + 3 <= n)
            {                   // Temperature
               temp_2_100 = d + 1;
               d += 3;
               continue;
            }
            if (*d == 0x03 && d + 3 <= n)
            {                   // Humidity
               hum_2_100 = d + 1;
               d += 3;
               continue;
            }
            if (*d == 0x2E && d + 2 <= n)
            {                   // Humidity
               hum_1 = d + 1;
               d += 2;
               continue;
            }
            if (*d == 0x57 && d + 2 <= n)
            {                   // Temperature
               temp_1 = d + 1;
               d += 2;
               continue;
            }
            if (*d == 0x58 && d + 2 <= n)
            {                   // Temperature
               temp_1_35 = d + 1;
               d += 2;
               continue;
            }
            if (*d == 0x45 && d + 3 <= n)
            {                   // Temperature
               temp_2_10 = d + 1;
               d += 3;
               continue;
            }
            if (*d == 0x12 && d + 3 <= n)
            {
               co2 = d[1] + (d[2] << 8);
               d += 3;
            }
            break;
         }
      } else if (*p >= 3 && p[1] == 0xFF)
      {                         // Custom type - with manufacturer code
         man = ((p[3] << 8) | p[2]);
         if (man == 0x757)
         {
            if (*p == 5 && p[4] == 0xF1)
               bat = p + 5;
            else if (*p == 6 && p[4] == 0xF2)
               volt = p + 5;
            else if (*p == 6 && p[4] == 0x12)
               temp_2_100 = p + 5;
         } else if (man == 0x0001 && *p == 9 && p[4] == 1 && p[5] == 1)
         {                      // GoveeLife - using man ID for Nokia, FFS
            if (!d)
               d = bleenv_find (&event->disc.addr, 1);
            if (d)
            {
               uint32_t v = (((p[6] & 0x7F) << 16) | (p[7] << 8) | p[8]);
               d->temp = (v / 1000) * 10 * (p[6] & 0x80 ? -1 : 1);      // C*100
               d->tempset = 1;
               d->hum = (v % 1000) * 10;        // Hum*100
               d->humset = 1;
               d->bat = p[9];
               d->batset = 1;
            }
         } else if (man == 0xEC88 && *p == 9)
         {                      // GoveeLife - using invalid man ID, FFS
            if (!d)
               d = bleenv_find (&event->disc.addr, 1);
            if (d)
            {
               uint32_t v = (((p[5] & 0x7F) << 16) | (p[6] << 8) | p[7]);
               d->temp = (v / 1000) * 10 * (p[5] & 0x80 ? -1 : 1);      // C*100
               d->tempset = 1;
               d->hum = (v % 1000) * 10;        // Hum*100
               d->humset = 1;
               d->bat = p[8];
               d->batset = 1;
            }
         } else if (man == 0xE9C)
         {                      // A&A
            if (*p == 9 && p[4] == 'F')
            {                   // Faikin
               if (!d)
                  d = bleenv_find (&event->disc.addr, 1);
               if (d)
               {
                  d->power = ((p[5] & 0x80) ? 1 : 0);
                  d->rad = ((p[5] & 0x40) ? 1 : 0);
                  d->mode = ((p[5] >> 3) & 7);
                  d->fan = (p[5] & 7);
                  if (p[6] & 0x80)
                  {
                     d->temp = (((p[6] & 0x1F) << 8) + p[7]) - 4000;
                     d->tempset = 1;
                  } else
                     d->tempset = 0;
                  if (p[6] & 0x40)
                  {
                     d->targetmin = 10 * (p[8] + 100);
                     d->targetminset = 1;
                  } else
                     d->targetminset = 0;
                  if (p[6] & 0x20)
                  {
                     d->targetmax = 10 * (p[9] + 100);
                     d->targetmaxset = 1;
                  } else
                     d->targetmaxset = 0;
                  d->faikinset = 1;
               }
            }
         } else
            man = 0;            // Unknown
      }
      p = n;
   }
   if (!d && !env && !man && !temp_2_100 && !temp_2_10 && !temp_1 && !temp_1_35)
      return 0;                 // No temp
   if (!d)
      d = bleenv_find (&event->disc.addr, 1);
   int diff (const char *a, const uint8_t * b, int len)
   {
      while (len > 0)
      {
         if (!(*a == *b || (*a == '_' && *b <= ' ')))
            return *a > *b ? 1 : -1;
         a++;
         b++;
         len--;
      }
      return 0;
   }
   if (name && (d->namelen != *name - 1 || diff (d->name, name + 2, d->namelen)) && (name[1] == 9 || !d->namefull))
   {                            // Update name
      memcpy (d->name, name + 2, d->namelen = *name - 1);
      d->name[d->namelen] = 0;
      for (int p = 0; p < d->namelen; p++)
         if (d->name[p] <= ' ')
            d->name[p] = '_';
      if (name[1] == 9)
         d->namefull = 1;
      d->changed = 1;
   }
   if (temp_2_100)
      d->temp = ((temp_2_100[1] << 8) | temp_2_100[0]); // C*100
   if (temp_2_10)
      d->temp = 10 * ((int16_t) ((temp_2_10[1] << 8) | temp_2_10[0]));  // C*100
   if (temp_1)
      d->temp = 100 * ((int8_t) temp_1[0]);
   if (temp_1_35)
      d->temp = 35 * ((int8_t) temp_1_35[0]);
   if ((temp_2_100 || temp_2_10 || temp_1 || temp_1_35) && !d->tempset)
   {
      d->tempset = 1;
      d->updated = 1;
   }
   if (bat)
   {
      d->bat = *bat;            // percent
      if (!d->batset)
      {
         d->batset = 1;
         d->updated = 1;
      }
   }
   if (volt)
      d->volt = ((volt[1] << 8) + volt[0]);     // mV
   if (hum_2_100)
      d->hum = ((hum_2_100[1] << 8) + hum_2_100[0]);    // Hum*100
   if (hum_1)
      d->hum = 100 * hum_1[0];  // Hum*100
   if ((hum_2_100 || hum_1) && !d->humset)
   {
      d->humset = 1;
      d->updated = 1;
   }
   if (co2)
   {
      d->co2 = co2;
      d->co2set = 1;
   } else
      d->co2set = 0;
   if (env)
   {
      if (*env == 18)
      {                         // Extended (custom)
         //ESP_LOG_BUFFER_HEX (event->disc.event_type == BLE_HCI_ADV_RPT_EVTYPE_SCAN_RSP ? "Rsp" : "Adv", event->disc.data, event->disc.length_data);
         d->temp = ((env[11] << 8) | env[10]);  // C * 100
         d->tempset = 1;
         d->hum = ((env[13] << 8) | env[12]);   // Hum %*100
         d->humset = 1;
         d->volt = ((env[15] << 8) | env[14]);  // mV
         d->voltset = 1;
         d->bat = env[16];      // %
         if (!d->batset)
         {
            d->batset = 1;
            d->updated = 1;
         }
         // counter
         // flags
         //ESP_LOGE (TAG, "Temp=%d hum=%d volt=%d bat=%d", d->temp, d->hum, d->volt, d->bat);
      }
   }
   d->rssi = event->disc.rssi;
   if (d->missing)
   {
      d->lastreport = 0;
      d->missing = 0;
      d->found = 1;
      d->updated = 1;
   }
   ESP_LOGD (TAG, "Temp \"%s\" T%d B%d V%d R%d", d->name, d->temp, d->bat, d->volt, d->rssi);
   return 0;
}

void
bleenv_expire (uint32_t missingtime)
{
   uint32_t now = uptime ();
   // Devices missing
   for (bleenv_t * d = bleenv; d; d = d->next)
      if (!d->missing && d->last + missingtime < now)
      {                         // Missing
         d->missing = 1;
         ESP_LOGI (TAG, "Missing %s %s", ble_addr_format (&d->addr), d->name);
      }
   // Devices found
   for (bleenv_t * d = bleenv; d; d = d->next)
      if (d->found)
      {
         d->found = 0;
         ESP_LOGI (TAG, "Found %s %s", ble_addr_format (&d->addr), d->name);
      }
}

void
bleenv_clean (void)
{
   if (ble_gap_disc_active ())
      return;                   // maybe use a mutex instead
   uint32_t now = uptime ();
   bleenv_t **dd = &bleenv;
   while (*dd)
   {
      bleenv_t *d = *dd;
      if (d->last + 300 < now)
      {
         ESP_LOGD (TAG, "Forget %s %s", ble_addr_format (&d->addr), d->name);
         *dd = d->next;
         free (d);
         continue;
      }
      dd = &d->next;
   }
}

const char *
ble_addr_format (ble_addr_t * a)
{
   static char buf[30];
   snprintf (buf, sizeof (buf), "%02X:%02X:%02X:%02X:%02X:%02X", a->val[5], a->val[4], a->val[3], a->val[2], a->val[1], a->val[0]);
   if (a->type == BLE_ADDR_RANDOM)
      strcat (buf, "(rand)");
   else if (a->type == BLE_ADDR_PUBLIC_ID)
      strcat (buf, "(pubid)");
   else if (a->type == BLE_ADDR_RANDOM_ID)
      strcat (buf, "(randid)");
   //else if (a->type == BLE_ADDR_PUBLIC) strcat(buf, "(pub)");
   return buf;
}

// --------------------------------------------------------------------------------
// Run BLE just for ELA

struct ble_hs_cfg;
struct ble_gatt_register_ctxt;
static int ble_gap_event (struct ble_gap_event *event, void *arg);

static void
ble_start_disc (void)
{
   struct ble_gap_disc_params disc_params = {
      .passive = active ? 0 : 1,
   };
   //ESP_LOGE(TAG,"Disc %s",active?"active":"passive");
   if (active)
      active--;
   if (ble_gap_disc (0 /* public */ , 1000, &disc_params, ble_gap_event, NULL))
      ESP_LOGE (TAG, "Discover failed to start");
}

static int
ble_gap_event (struct ble_gap_event *event, void *arg)
{
   switch (event->type)
   {
   case BLE_GAP_EVENT_DISC:
      {
         bleenv_gap_disc (event);
         break;
      }
   case BLE_GAP_EVENT_DISC_COMPLETE:
      {
         ble_start_disc ();
         break;
      }
   default:
      ESP_LOGD (TAG, "BLE event %d", event->type);
      break;
   }

   return 0;
}

static uint8_t ble_addr_type;
static void
ble_on_sync (void)
{
   ESP_LOGI (TAG, "BLE Discovery Started");
   int rc;

   rc = ble_hs_id_infer_auto (0, &ble_addr_type);
   assert (rc == 0);

   uint8_t addr_val[6] = { 0 };
   rc = ble_hs_id_copy_addr (ble_addr_type, addr_val, NULL);

   ble_start_disc ();
}

static void
ble_on_reset (int reason)
{
}

static void
ble_task (void *param)
{
   ESP_LOGI (TAG, "BLE Host Task Started");
   /* This function will return only when nimble_port_stop() is executed */
   nimble_port_run ();

   nimble_port_freertos_deinit ();
}

void
bleenv_run (void)
{                               // Just run BLE for ELA only
   REVK_ERR_CHECK (esp_wifi_set_ps (WIFI_PS_MIN_MODEM));        /* default mode, but library may have overridden, needed for BLE at same time as wifi */
   nimble_port_init ();

   /* Initialize the NimBLE host configuration */
   ble_hs_cfg.sync_cb = ble_on_sync;
   ble_hs_cfg.reset_cb = ble_on_reset;
   ble_hs_cfg.sm_sc = 1;
   ble_hs_cfg.sm_mitm = 0;
   ble_hs_cfg.sm_bonding = 1;
   ble_hs_cfg.sm_io_cap = BLE_SM_IO_CAP_NO_IO;

   /* Start the task */
   nimble_port_freertos_init (ble_task);
   ESP_LOGI (TAG, "Starting ELA monitoring");
}

static uint8_t
add_name (uint8_t * data, uint8_t p, uint8_t len, const char *name)
{
   if (!name || !*name || p + len + 2 >= MAX_ADV)
      return p;
   // Name
   int8_t l = strlen (name);
   int8_t space = MAX_ADV - p - len - 2;
   if (l > space)
   {                            // Shortened
      l = space;
      data[p++] = (l + 1);
      data[p++] = (8);
   } else
   {                            // Full
      data[p++] = (l + 1);
      data[p++] = (9);
   }
   while (l--)
      data[p++] = (*name++);
   return p;
}


static void
ble_adv (const char *name, uint8_t * data, uint8_t len)
{
   //ESP_LOG_BUFFER_HEX_LEVEL ("ADV", data, len, ESP_LOG_ERROR);
   ble_gap_adv_set_data (data, len);
   uint8_t rsp[MAX_ADV],
     p = 0;
   p = add_name (rsp, p, 0, name);
   //ESP_LOG_BUFFER_HEX_LEVEL ("RSP", rsp, p, ESP_LOG_ERROR);
   ble_gap_adv_rsp_set_data (rsp, p);
   if (!ble_gap_adv_active ())
   {
      struct ble_gap_adv_params adv_params = { 0 };
      adv_params.conn_mode = BLE_GAP_CONN_MODE_NON;
      adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
      int e = ble_gap_adv_start (BLE_OWN_ADDR_PUBLIC, NULL, BLE_HS_FOREVER, &adv_params, NULL, NULL);
      ESP_LOGD ("BLE", "Adv started %d", e);
   }
}

void
bleenv_bthome1 (const char *name, float c, float rh, uint16_t co2, float lux)
{                               // Set up / update advertising BTHome1 format
   uint8_t data[MAX_ADV],
     p = 0;
   data[p++] = 2;               // Len
   data[p++] = 1;               // Flags
   data[p++] = 6;               // Discoverable / no BR
   uint8_t len = 4;
   if (!isnan (c))
      len += 4;
   if (rh)
      len += 4;
   if (co2)
      len += 4;
   if (lux)
      len += 5;
   if (p + len > sizeof (data))
      return;                   // should not happen
   p = add_name (data, p, len, name);
   data[p++] = len - 1;
   data[p++] = 0x16;            // BTHome1
   data[p++] = 0x1C;
   data[p++] = 0x18;
   if (!isnan (c))
   {
      int16_t C = c * 100;
      data[p++] = 0x23;         // temp, C*100
      data[p++] = 2;
      data[p++] = C;
      data[p++] = C >> 8;
   }
   if (rh)
   {
      uint16_t H = rh * 100;
      data[p++] = 0x03;         // humidity, RH*100
      data[p++] = 3;
      data[p++] = H;
      data[p++] = H >> 8;
   }
   if (co2)
   {
      data[p++] = 0x12;
      data[p++] = 2;
      data[p++] = co2;
      data[p++] = co2 >> 8;
   }
   if (lux)
   {
      uint32_t L = lux * 100;
      data[p++] = 0x05;
      data[p++] = 3;
      data[p++] = L;
      data[p++] = L >> 8;
      data[p++] = L >> 16;
   }
   ble_adv (name, data, p);
}

void
bleenv_bthome2 (const char *name, float c, float rh, uint16_t co2, float lux)
{                               // Set up / update advertising BTHome2 format
   uint8_t data[MAX_ADV],
     p = 0;
   data[p++] = 2;               // Len
   data[p++] = 1;               // Flags
   data[p++] = 6;               // Discoverable / no BR
   uint8_t len = 5;
   if (!isnan (c))
      len += 3;
   if (rh)
      len += 3;
   if (co2)
      len += 3;
   if (lux)
      len += 4;
   if (p + len > sizeof (data))
      return;                   // should not happen
   data[p++] = len - 1;
   data[p++] = 0x16;            // BTHome2
   data[p++] = 0xD2;
   data[p++] = 0xFC;
   data[p++] = 0x40;            // v2

   if (!isnan (c))
   {
      int16_t C = c * 100;
      data[p++] = 0x02;         // temp, C*100
      data[p++] = C;
      data[p++] = C >> 8;
   }
   if (rh)
   {
      uint16_t H = rh * 100;
      data[p++] = 0x03;         // humidity, RH*100
      data[p++] = H;
      data[p++] = H >> 8;
   }
   if (co2)
   {
      data[p++] = 0x12;
      data[p++] = co2;
      data[p++] = co2 >> 8;
   }
   if (lux)
   {
      uint32_t L = lux * 100;
      data[p++] = 0x05;
      data[p++] = L;
      data[p++] = L >> 8;
      data[p++] = L >> 16;
   }
   p = add_name (data, p, 0, name);
   ble_adv (name, data, p);
}

void
bleenv_faikin (const char *name, float c, float targetmin, float targetmax, uint8_t power, uint8_t rad, uint8_t mode, uint8_t fan)
{                               // Set up / update advertising Faikin format
   if (mode > 7)
      mode = 0;
   if (fan > 7)
      fan = 0;
   if (!isnan (c))
   {
      if (c < -40)
         c = -40;
      else if (c > 40)
         c = 40;
   }
   if (!isnan (targetmin))
   {
      if (targetmin < 10)
         targetmin = 10;
      else if (targetmin > 35)
         targetmin = 35;
   }
   if (!isnan (targetmax))
   {
      if (targetmax < 10)
         targetmax = 10;
      else if (targetmax > 35)
         targetmax = 35;
   }
   uint8_t data[MAX_ADV],
     p = 0;
   data[p++] = 2;               // Len
   data[p++] = 1;               // Flags
   data[p++] = 6;               // Discoverable / no BR
   data[p++] = 9;               // Len
   data[p++] = 0xFF;            // Manufacturer specific
   data[p++] = 0x9C;            // A&A
   data[p++] = 0x0E;
   data[p++] = 'F';             // Message
   data[p++] = (power ? 0x80 : 0) + (rad ? 0x40 : 0) + (mode << 3) + fan;
   int16_t T = 0;
   if (!isnan (c))
      T = 0x8000 + (c + 40) * 100;      // 13 bits - top bits are if temp/targets set
   if (!isnan (targetmin))
      T |= 0x4000;
   if (!isnan (targetmax))
      T |= 0x2000;
   data[p++] = T >> 8;
   data[p++] = T;
   T = (isnan (targetmin) ? 0 : (targetmin - 10) * 10);
   data[p++] = T;
   T = (isnan (targetmax) ? 0 : (targetmax - 10) * 10);
   data[p++] = T;
   p = add_name (data, p, 0, name);
   ble_adv (name, data, p);
}

#endif