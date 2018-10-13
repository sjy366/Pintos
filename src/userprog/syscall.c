#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
#include "devices/shutdown.h"
#include <console.h>

typedef int pid_t;
static void syscall_handler (struct intr_frame *);

bool is_valid_addr(const uint8_t *uaddr);
void syscall_halt (void);
void syscall_exit (int status);
pid_t syscall_exec (const char *cmd_line);
int syscall_wait (pid_t pid);
int syscall_read (int fd, void *buffer, unsigned size);
int syscall_write (int fd, const void *buffer, unsigned size);

bool syscall_create (const char *file, unsigned initial_size);
bool syscall_remove (const char *file);
int syscall_open (const char *file);
int syscall_filesize (int fd);
void syscall_seek (int fd, unsigned position);
unsigned syscall_tell (int fd);
void syscall_close (int fd);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  int syscall_num;

  if(is_valid_addr(f->esp)) syscall_exit(-1);
  else syscall_num = *(int*)f->esp;

  switch(syscall_num)
  {
    case SYS_HALT:
      syscall_halt();
      break;
    case SYS_EXIT:
      int status;
      if(is_valid_addr(f->esp + 4)) syscall_exit(-1);
      else status = *(int*)(f->esp + 4);
      syscall_exit(status);
      break;
    case SYS_EXEC:
      const char *cmd_line;
      if(is_valid_addr(f->esp + 4)) syscall_exit(-1);
      else cmd_line = *(const char*)(f->esp + 4);
      f->eax = syscall_exec(cmd_line);
      break;
    case SYS_WAIT:
      pid_t pid;
      if(is_valid_addr(f->esp + 4)) syscall_exit(-1);
      else pid = *(pid_t*)(f->esp + 4);
      f->eax = syscall_wait(pid);
      break;
    case SYS_READ:
      int fd;
      void *buffer;
      unsigned size;
      if(is_valid_addr(f->esp + 4)) syscall_exit(-1);
      else fd = *(int*)(f->esp + 4);
      if(is_valid_addr(f->esp + 8)) syscall_exit(-1);
      else buffer = *(void**)(f->esp + 8);
      if(is_valid_addr(f->esp + 12)) syscall_exit(-1);
      else size = *(unsigned*)(f->esp + 12);
      f->eax = syscall_read(fd, buffer, size);
      break;
    case SYS_WRITE:
      int fd;
      const void *buffer;
      unsigned size;
      if(is_valid_addr(f->esp + 4)) syscall_exit(-1);
      else fd = *(int*)(f->esp + 4);
      if(is_valid_addr(f->esp + 8)) syscall_exit(-1);
      else buffer = *(const void**)(f->esp + 8);
      if(is_valid_addr(f->esp + 12)) syscall_exit(-1);
      else size = *(unsigned*)(f->esp + 12);
      f->eax = syscall_write(fd, buffer, size);
      break;
    case SYS_CREATE:
      break;
    case SYS_REMOVE:
      break;
    case SYS_OPEN:
      break;
    case SYS_FILESIZE:
      break;
    case SYS_SEEK:
      break;
    case SYS_TELL:
      break;
    case SYS_CLOSE:
      break;
    case SYS_MMAP:
      break;
    case SYS_MUNMAP:
      break;
    case SYS_CHDIR:
      break;
    case SYS_MKDIR:
      break;
    case SYS_READDIR:
      break;
    case SYS_ISDIR:
      break;
    case SYS_INUMBER:
      break;
  }
}

bool
is_valid_addr (const uint8_t *uaddr)
{
  struct thread *t = thread_current();
  if(uaddr == NULL) return 0;
  if(is_kernel_vaddr(uaddr)) return 0;
  if(pagedir_get_page(t->pagedir, uaddr) == NULL) return 0;
  return 1;
}
void
syscall_halt (void)
{
  shutdown_power_off();
}
void
syscall_exit (int status)
{

}
pid_t
syscall_exec (const char *cmd_line)
{

}
int
syscall_wait (pid_t pid)
{

}
int
syscall_read (int fd, void *buffer, unsigned size)
{
  int i;
  if(fd == 0)
    {
      for(i = 0; i < size; i++)
        {
          ((uint8_t*)buffer)[i] = input_getc();
          if(((uint8_t*)buffer)[i] == 0) return i;
        }
      return i;
    }
  else return -1;
}
int
syscall_write (int fd, const void *buffer, unsigned size)
{
  if(fd == 1)
    {
      putbuf(buffer, size);
      return size;
    }
  else return -1;
}