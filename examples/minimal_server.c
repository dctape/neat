#include <neat.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/*
 * Note: Do not edit this file without updating the line numbers in the tutorial
 * in the documentation.
 */

static neat_error_code
on_readable(struct neat_flow_operations *opCB)
{
    uint32_t bytes_read = 0;
    unsigned char buffer[32];

    if (neat_read(opCB->ctx, opCB->flow, buffer, 31, &bytes_read, NULL, 0) == NEAT_OK) {
        buffer[bytes_read] = 0;
        fprintf(stdout, "Read %u bytes:\n%s", bytes_read, buffer);
    }

    return NEAT_OK;
}

static neat_error_code
on_writable(struct neat_flow_operations *opCB)
{
    const unsigned char message[] = "Hello, this is NEAT!";
    neat_write(opCB->ctx, opCB->flow, message, 20, NULL, 0);
    opCB->on_writable = NULL; 
    return NEAT_OK;
}

static neat_error_code
on_all_written(struct neat_flow_operations *opCB)
{
    neat_close(opCB->ctx, opCB->flow);
    return NEAT_OK;
}

static neat_error_code
on_connected(struct neat_flow_operations *opCB)
{
    opCB->on_writable    = on_writable;
    opCB->on_all_written = on_all_written;
    neat_set_operations(opCB->ctx, opCB->flow, opCB);

    return NEAT_OK;
}

int
main(int argc, char *argv[])
{
    struct neat_ctx *ctx;
    struct neat_flow *flow;
    struct neat_flow_operations ops;

    ctx  = neat_init_ctx();
    flow = neat_new_flow(ctx);
    memset(&ops, 0, sizeof(ops));

    ops.on_readable  = on_readable;
    ops.on_connected = on_connected;
    neat_set_operations(ctx, flow, &ops);

    if (neat_accept(ctx, flow, 5000, NULL, 0)) {
        fprintf(stderr, "neat_accept failed\n");
        return EXIT_FAILURE;
    }

    neat_start_event_loop(ctx, NEAT_RUN_DEFAULT);

    return EXIT_SUCCESS;
}
