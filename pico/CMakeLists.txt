cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(pico-peristaltic-tester C CXX ASM)
pico_sdk_init()

if (NOT TARGET tinyusb_device)
  message(FATAL_ERROR "picosdk's TinyUSB submodule is required for serial output to USB.")
endif()

add_executable(pico-peristaltic-tester
  main.cpp
  )

target_compile_features(pico-peristaltic-tester PUBLIC
  cxx_std_17)

ignore_library_warnings(pico_stdlib)
ignore_library_warnings(pico_cyw43_arch_none)
target_link_libraries(pico-peristaltic-tester PUBLIC
  pico_stdlib
  )
# output via USB, not UART
pico_enable_stdio_uart(deskstock 0)
pico_enable_stdio_usb(deskstock 1)

pico_add_extra_outputs(deskstock) # create uf2 and other stuff I don't care about
