#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <string>

void open_and_write(const std::string &filename, const size_t block_size,
                    const char fill_byte) {
  char *block = (char *)malloc(block_size);

  std::fill(block, block + block_size, fill_byte);

  int flags = (O_CREAT | O_TRUNC | O_RDWR);

  int fd = open(filename.c_str(), flags, 0644);
  if (fd != -1) {
    ssize_t ret = -1;
    ret = write(fd, block, block_size);

    if (ret < 0) {
      std::cerr << "Error when writing file " << filename << "; errno " << errno
                << "\n ";
    }
  } else {
    std::cerr << "Error when opening file " << filename << "; errno " << errno
              << "\n";
  }
  int err_close = close(fd);
  if (err_close != 0) {
    std::cerr << "Error when closing file " << filename << "; errno " << errno
              << "\n";
  }

  free(block);
}

int main(int argc, char const *argv[]) {
  // std::cerr << "Small write test ...";
  // open_and_write("small_block.bin", 1024, 0x01);
  // std::cerr << "done\n";

  // std::cerr << "2^30 write test ...";
  // open_and_write("2_30_block.bin", 1UL << 30, 0x02);
  // std::cerr << "done\n";

  std::cerr << "2^31-1 write test ...";
  open_and_write("2_31_1_block.bin", (1UL << 31) - 1, 0x02);
  std::cerr << "done\n";

  std::cerr << "2^31 write test ...";
  open_and_write("2_31_block.bin", 1UL << 31, 0x03);
  std::cerr << "done\n";

  // std::cerr << "2^32 write test ...";
  // open_and_write("2_32_block.bin", 1UL << 32, 0x04);
  // std::cerr << "done\n";

  return 0;
}
