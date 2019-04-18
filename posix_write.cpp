#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <string>

bool write_wrapper(int fd, const void *buf, size_t nbyte) {
  constexpr size_t LIMIT_2GB = 1UL << 31;
  constexpr size_t LIMIT_1GB = 1UL << 30;

  const char *src = reinterpret_cast<const char *>(buf);
  size_t left = nbyte;

  while (left != 0) {
    size_t bytes_to_write = left;
    if (bytes_to_write >= LIMIT_2GB) {
      bytes_to_write = LIMIT_1GB;
    }
    ssize_t done = write(fd, src, bytes_to_write);
    if (done < 0) {
      if (errno == EINTR) {
        continue;
      }
      return false;
    }
    left -= done;
    src += done;
  }
  return true;
}

bool pwrite_wrapper(int fd, const void *buf, size_t nbyte, off_t offset) {
  constexpr size_t LIMIT_2GB = 1UL << 31;
  constexpr size_t LIMIT_1GB = 1UL << 30;

  const char *src = reinterpret_cast<const char *>(buf);
  size_t left = nbyte;

  while (left != 0) {
    size_t bytes_to_write = left;
    if (bytes_to_write >= LIMIT_2GB) {
      bytes_to_write = LIMIT_1GB;
    }

    ssize_t done = pwrite(fd, src, bytes_to_write, offset);
    if (done < 0) {
      if (errno == EINTR) {
        continue;
      }
      return false;
    }
    left -= done;
    offset += done;
    src += done;
  }

  return true;
}

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

void open_and_pwrite(const std::string &filename, const size_t block_size,
                     const char fill_byte) {
  char *block = (char *)malloc(block_size);

  std::fill(block, block + block_size, fill_byte);

  int flags = (O_CREAT | O_TRUNC | O_RDWR);

  int fd = open(filename.c_str(), flags, 0644);
  if (fd != -1) {
    ssize_t ret = -1;
    ret = pwrite(fd, block, block_size, 10);

    if (ret < 0) {
      std::cerr << "Error when writing file (with pwrite) " << filename
                << "; errno " << errno << "\n ";
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

void open_and_write_wrapper(const std::string &filename,
                            const size_t block_size, const char fill_byte) {
  char *block = (char *)malloc(block_size);

  std::fill(block, block + block_size, fill_byte);

  int flags = (O_CREAT | O_TRUNC | O_RDWR);

  int fd = open(filename.c_str(), flags, 0644);
  if (fd != -1) {
    if (!write_wrapper(fd, block, block_size)) {
      std::cerr << "Error when writing file (with write wrapper)" << filename
                << "; errno " << errno << "\n ";
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

void open_and_pwrite_wrapper(const std::string &filename,
                             const size_t block_size, const char fill_byte) {
  char *block = (char *)malloc(block_size);

  std::fill(block, block + block_size, fill_byte);

  int flags = (O_CREAT | O_TRUNC | O_RDWR);

  int fd = open(filename.c_str(), flags, 0644);
  if (fd != -1) {

    if (!pwrite_wrapper(fd, block, block_size, 10)) {
      std::cerr << "Error when writing file (with pwrite wrapper) " << filename
                << "; errno " << errno << "\n ";
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

  // std::cerr << "2^31-1 write test ...";
  // open_and_write("2_31_1_block.bin", (1UL << 31) - 1, 0x02);
  // std::cerr << "done\n";

  // std::cerr << "2^31 write test ...";
  // open_and_write("2_31_block.bin", 1UL << 31, 0x03);
  // std::cerr << "done\n";

  std::cerr << "2^31-1 write wrapper test ...";
  open_and_write_wrapper("2_31_1_block.bin", (1UL << 31) - 1, 0x02);
  std::cerr << "done\n";

  std::cerr << "2^31 write wrapper test ...";
  open_and_write_wrapper("2_31_block.bin", 1UL << 31, 0x03);
  std::cerr << "done\n";

  // std::cerr << "2^31-1 pwrite test ...";
  // open_and_pwrite("2_31_1_block.bin", (1UL << 31) - 1, 0x02);
  // std::cerr << "done\n";

  std::cerr << "2^31 pwrite test ...";
  open_and_pwrite("2_31_block.bin", 1UL << 31, 0x03);
  std::cerr << "done\n";

  std::cerr << "2^32 write test ...";
  open_and_write("2_32_block.bin", 1UL << 32, 0x04);
  std::cerr << "done\n";

  std::cerr << "2^31 pwrite wrapper test ...";
  open_and_pwrite_wrapper("2_31_block.bin", 1UL << 31, 0x03);
  std::cerr << "done\n";

  return 0;
}
