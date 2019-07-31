# Source files
CSRCS = \
  main/main.c\
  main/storage_task.c\
  main/usb_descriptors.c\
  main/usb_specific_request.c\
  \
  modules/control_access/ctrl_access.c\
  modules/scheduler/scheduler.c\
  modules/scsi_decoder/scsi_decoder.c\
  modules/usb/device_chap9/usb_standard_request.c\
  modules/usb/device_chap9/usb_device_task.c\
  modules/usb/usb_task.c\
  \
  lib_mcu/usb/usb_drv.c\
  lib_mcu/power/power_drv.c\
  lib_mcu/wdt/wdt_drv.c\
  \
  lib_mem/cf/cf_mem.c\
  lib_mem/cf/cf.c\
  lib_mem/cf/fat12_data.c\
  lib_mem/cf/boot_choice.c\

# Assembler source files
ASSRCS = \
  lib_mem/cf/bootfile_template.S\

