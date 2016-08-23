#include "contiki.h"
#include "net/rime/rime.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "/home/user/contiki/platform/micaz/dev/sensors/mts300.c"
#include <stdio.h>

/*---------------------------------------------------------------------------*/
PROCESS(example_unicast_process, "Example unicast");
AUTOSTART_PROCESSES(&example_unicast_process);
/*---------------------------------------------------------------------------*/
struct value{
	uint16_t moisture,temperature, light;
};

static void recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{
  leds_on(LEDS_ALL);
  struct value *val = (struct value*)packetbuf_dataptr();
  printf("unicast message received from %d.%d\n", from->u8[0], from->u8[1]);
  printf("Light :%d\n",val->light);
  printf("Tempreature : %d\n",val->temperature);
    
}
static const struct unicast_callbacks unicast_callbacks = {recv_uc};
static struct unicast_conn uc;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_unicast_process, ev, data)
{
  PROCESS_EXITHANDLER(unicast_close(&uc);)
    
  PROCESS_BEGIN();

  unicast_open(&uc, 146, &unicast_callbacks);

  while(1) {
    static struct etimer et;
    //linkaddr_t addr;
    
    etimer_set(&et, CLOCK_SECOND);
    
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    /*
    packetbuf_copyfrom("Hello", 5);
    addr.u8[0] = 1;
    addr.u8[1] = 0;
    if(!linkaddr_cmp(&addr, &linkaddr_node_addr)) {
      unicast_send(&uc, &addr);
    }
    */
    leds_off(LEDS_ALL);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
