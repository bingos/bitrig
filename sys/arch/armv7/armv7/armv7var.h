#include <armv7/armv7/armv7_devices.h>

extern struct bus_space armv7_bs_tag;
extern struct bus_space armv7_a4x_bs_tag;

void armv7_intr_bootstrap(vaddr_t);

/* Passed as third arg to attach functions. */
struct arm_attach_args {
	struct arm_dev		*aa_dev;
	bus_space_tag_t		 aa_iot;
	bus_dma_tag_t		 aa_dmat;
};

struct arm_dev *arm_find_dev(const char *, int);

extern uint32_t			 board_id;
extern struct arm_board		*board;
