target_sources(
  shvav-8
  PRIVATE
  main.cpp
  app.cpp
  fps.cpp

  emulation/shvav8.cpp
  emulation/frame_buffer.cpp

  rendering/window.cpp
  rendering/renderer.cpp
  rendering/shader.cpp

  audio/beeper.cpp
)
