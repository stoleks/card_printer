#pragma once

#include <SFML/System/Clock.hpp>
#include <spdlog/spdlog.h>

/**
 * For cheap profiling
 */
void printTime (
  const sf::Clock& clock,
  const std::string& msg = "")
{
  const auto elapsed = clock.getElapsedTime ().asMilliseconds ();
  spdlog::warn ("Time elasped: {} ms {}", elapsed, msg);
}
