#pragma once

#include "GraphicalParts.h"
#include "Informations.h"

struct Card {
  Card () = default;
  Card (const CardFormat& f, const CardIdentifier& i, const GraphicalParts& g)
    : format (f), identifier (i), graphics (g)
  {}
  // data
  CardFormat format = {};
  CardTemplate templat = {};
  CardIdentifier identifier = {};
  GraphicalParts graphics = {};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Card, format, templat, identifier, graphics)
