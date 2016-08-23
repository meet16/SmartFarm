#include "contiki.h"
#include "net/rime/rime.h"
#include "lib/list.h"
#include "lib/memb.h"
#include "lib/random.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "/home/user/contiki/platform/micaz/dev/sensors/mts300.c"
#include "/home/user/contiki/platform/micaz/dev/sensors/battery-sensor.c"
#include <stdio.h>

#define CHANNEL 135
#define NEIGHBOR_TIMEOUT 60 * CLOCK_SECOND
#define MAX_NEIGHBORS 16

struct example_neighbor {
  struct example_neighbor *next;
  linkaddr_t addr;
  struct ctimer ctimer;
};


struct value{
	uint16_t moisture,temperature, light;
	int count,battery;
};

LIST(neighbor_table);
MEMB(neighbor_mem, struct example_neighbor, MAX_NEIGHBORS);
/*---------------------------------------------------------------------------*/
PROCESS(example_multihop_process, "multihop send");
AUTOSTART_PROCESSES(&example_multihop_process);
/*---------------------------------------------------------------------------*/
/*
 * This function is called by the ctimer present in each neighbor
 * table entry. The function removes the neighbor from the table
 * because it has become too old.
 */
static void
remove_neighbor(void *n)
{
  struct example_neighbor *e = n;

  list_remove(neighbor_table, e);
  memb_free(&neighbor_mem, e);
}
/*---------------------------------------------------------------------------*/
/*
 * This function is called when an incoming announcement arrives. The
 * function checks the neighbor table to see if the neighbor is
 * already present in the list. If the neighbor is not present in the
 * list, a new neighbor table entry is allocated and is added to the
 * neighbor table.
 */
static void
received_announcement(struct announcement *a,
                      const linkaddr_t *from,
		      uint16_t id, uint16_t value)
{
  struct example_neighbor *e;

  /*  printf("Got announcement from %d.%d, id %d, value %d\n",
      from->u8[0], from->u8[1], id, value);*/

  /* We received an announcement from a neighbor so we need to update
     the neighbor list, or add a new entry to the table. */
  for(e = list_head(neighbor_table); e != NULL; e = e->next) {
    if(linkaddr_cmp(from, &e->addr)) {
      /* Our neighbor was found, so we update the timeout. */
      ctimer_set(&e->ctimer, NEIGHBOR_TIMEOUT, remove_neighbor, e);
      return;
    }
  }

  /* The neighbor was not found in the list, so we add a new entry by
     allocating memory from the neighbor_mem pool, fill in the
     necessary fields, and add it to the list. */
  e = memb_alloc(&neighbor_mem);
  if(e != NULL) {
    linkaddr_copy(&e->addr, from);
    list_add(neighbor_table, e);
    ctimer_set(&e->ctimer, NEIGHBOR_TIMEOUT, remove_neighbor, e);
  }
  leds_on(LEDS_RED);
}
static struct announcement example_announcement;
/*---------------------------------------------------------------------------*/
/*
 * This function is called at the final recepient of the message.
 */
static void
recv(struct multihop_conn *c, const linkaddr_t *sender,
     const linkaddr_t *prevhop,
     uint8_t hops)
{
  struct value *val = (struct value*)packetbuf_dataptr();
  printf("multihop message received from %d.%d\n", prevhop->u8[0], prevhop->u8[1]);
  printf("multihop message sender from %d.%d\n", sender->u8[0], sender->u8[1]);
  printf("Light :%d\n",val->light);
  printf("Tempreature : %d\n",val->temperature);
  printf("----------------Count : %d\n",val->count);
  printf("----------------Battery : %d\n",val->battery);
  leds_on(LEDS_GREEN);
}
/*
 * This function is called to forward a packet. The function picks a
 * random neighbor from the neighbor list and returns its address. The
 * multihop layer sends the packet to this address. If no neighbor is
 * found, the function returns NULL to signal to the multihop layer
 * that the packet should be dropped.
 */
static linkaddr_t *
forward(struct multihop_conn *c,
	const linkaddr_t *originator, const linkaddr_t *dest,
	const linkaddr_t *prevhop, uint8_t hops)
{
  /* Find a random neighbor to send to. */
  int num, i;
  struct example_neighbor *n;

  if(list_length(neighbor_table) > 0) {
    num = random_rand() % list_length(neighbor_table);
    i = 0;
    for(n = list_head(neighbor_table); n != NULL && i != num; n = n->next) {
      ++i;
    }
    if(n != NULL) {
      printf("%d.%d: Forwarding packet to %d.%d (%d in list), hops %d\n",
	     linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1],
	     n->addr.u8[0], n->addr.u8[1], num,
	     packetbuf_attr(PACKETBUF_ATTR_HOPS));
      return &n->addr;
    }
  }
  printf("%d.%d: did not find a neighbor to foward to\n",
	 linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1]);
	 
  leds_on(LEDS_YELLOW);
  return NULL;
}
static const struct multihop_callbacks multihop_call = {recv, forward};
static struct multihop_conn multihop;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_multihop_process, ev, data)
{
  struct value val;
  
  val.count = 0;
  
  PROCESS_EXITHANDLER(multihop_close(&multihop);)
    
  PROCESS_BEGIN();

  /* Initialize the memory for the neighbor table entries. */
  memb_init(&neighbor_mem);

  /* Initialize the list used for the neighbor table. */
  list_init(neighbor_table);

  /* Open a multihop connection on Rime channel CHANNEL. */
  multihop_open(&multihop, CHANNEL, &multihop_call);

  /* Register an announcement with the same announcement ID as the
     Rime channel we use to open the multihop connection above. */
  announcement_register(&example_announcement,
			CHANNEL,
			received_announcement);

  /* Set a dummy value to start sending out announcments. */
  announcement_set_value(&example_announcement, 0);
  
  /* Loop forever, send a packet when the button is pressed. */
  while(1) {
    linkaddr_t to;
    
    static struct etimer et;
    
    etimer_set(&et, CLOCK_SECOND);
    
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    /* Wait until we get a sensor event with the button sensor as data. */
 
    /* Copy val structure to the packet buffer. */
    val.moisture = 1024;
    val.temperature = get_temp();
    val.light = get_light();
    //SENSORS_ACTIVATE(battery_sensor);
    val.battery = battery_sensor.value(0);
    //SENSORS_DEACTIVATE(battery_sensor);
        
    packetbuf_copyfrom(&val, 10);

    val.count++;
    /* Set the Rime address of the final receiver of the packet to
       66.116.*/
    to.u8[0] = 66;
    to.u8[1] = 116;
    
    /* Send the packet. */
    multihop_send(&multihop, &to);
    leds_off(LEDS_ALL);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
