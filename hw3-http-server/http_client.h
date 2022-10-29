#pragma once

#include <stdint.h>

static void init_params(int argc, char **argv);
static void start_client();
static char *generate_header_for_request(char *path);
static void send_and_recv_data(char *data);
