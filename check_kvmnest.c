#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/kvm.h>

struct vm {
	int sys_fd;
	int fd;
};

int main() {
	int api_ver;

	struct vm *vm = malloc(sizeof(struct vm));
	if(vm == NULL) {
		fprintf(stderr, "failed malloc(vm)\n");
		exit(1);
	}

	vm->sys_fd = open("/dev/kvm", O_RDWR);
	if(vm->sys_fd < 0) {
		perror("open /dev/kvm");
		exit(1);
	}

	api_ver = ioctl(vm->sys_fd, KVM_GET_API_VERSION, 0);
	if(api_ver < 0) {
		perror("KVM_GET_API_VERSION");
		exit(1);
	}

	if(api_ver != KVM_API_VERSION) {
		fprintf(stderr, "invalid kvm api version returned: %d expected %d\n", api_ver, KVM_API_VERSION);
		exit(1);
	}

	vm->fd = ioctl(vm->sys_fd, KVM_CREATE_VM, 0);
	if(vm->fd < 0) {
		perror("KVM_CREATE_VM");
		exit(1);
	}

	if(ioctl(vm->fd, KVM_CHECK_EXTENSION, KVM_CAP_NESTED_STATE) == 0) {
		printf("KVM NESTING NOT SUPPORTED BY CPU\n");
		exit(1);
	}
	printf("KVM NESTING SUPPORTED BY CPU\n");
	exit(0);
}

