/*
 * inci driver
 * Copyright (C) 2008 i Bilisim Teknolojileri Ltd. Sti.
 *
 */

#include <stddef.h>


class SeriPort {
public:
	SeriPort(const char *portdev, unsigned int speed);
	~SeriPort();

	int read(char *buffer, size_t len);
	int write(const char *buffer, size_t len);
	void flush_input();

	int is_ok;

private:
	int fd;
};
