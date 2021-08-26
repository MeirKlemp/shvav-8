target_sources(
  shvav-8
  PRIVATE
  main.cpp
  app.cpp

  emulation/shvav8.cpp
  emulation/frame_buffer.cpp
  emulation/exceptions.cpp

  rendering/window.cpp
  rendering/renderer.cpp
)
