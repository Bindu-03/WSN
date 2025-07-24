#include "contiki.h"
#include "net/rime/rime.h"
#include "lib/random.h"
#include "sys/etimer.h"
#include "sys/energest.h"

#include <stdio.h>
#include <string.h>

#define ENERGY_THRESHOLD 50
#define MAX_DISTANCE     300
#define TRAFFIC_LIMIT    10

PROCESS(myai_routing_process, "AI Routing Protocol");
AUTOSTART_PROCESSES(&myai_routing_process);

static struct etimer periodic_timer;
static uint16_t traffic_count = 0;
static uint16_t residual_energy = 100;

static uint16_t get_distance_to_sink() {
  return (random_rand() % MAX_DISTANCE);
}

static uint8_t evaluate_ai_decision(uint16_t energy, uint16_t distance, uint16_t traffic) {
  uint8_t score = 0;
  if(energy > ENERGY_THRESHOLD) score += 2;
  if(distance < 150) score += 2;
  if(traffic < TRAFFIC_LIMIT) score += 1;
  return score;
}

static void forward_packet_if_best() {
  uint16_t distance = get_distance_to_sink();
  uint8_t score = evaluate_ai_decision(residual_energy, distance, traffic_count);

  if(score >= 4) {
    printf("[AI-Routing] Forwarding packet. Energy: %u, Distance: %u, Score: %u\n",
           residual_energy, distance, score);
  } else {
    printf("[AI-Routing] Dropping/Deferring packet. Score: %u\n", score);
  }

  traffic_count++;
  residual_energy -= 1;
}

PROCESS_THREAD(myai_routing_process, ev, data)
{
  PROCESS_BEGIN();

  printf("[AI-Routing] Starting AI-based routing...\n");
  etimer_set(&periodic_timer, CLOCK_SECOND * 10);

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    forward_packet_if_best();
    etimer_reset(&periodic_timer);
  }

  PROCESS_END();
}

