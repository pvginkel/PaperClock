#include "includes.h"
#include "mdi-icons.h"

const char* classify_weather_image(const string& image) {
  if (image == "zonnig") {
    return MDI_WEATHER_SUNNY;
  }
  if (image == "bliksem") {
    return MDI_WEATHER_LIGHTNING;
  }
  if (image == "regen") {
    return MDI_WEATHER_RAINY;
  }
  if (image == "buien") {
    return MDI_WEATHER_POURING;
  }
  if (image == "hagel") {
    return MDI_WEATHER_HAIL;
  }
  if (image == "mist") {
    return MDI_WEATHER_FOG;
  }
  if (image == "sneeuw") {
    return MDI_WEATHER_SNOWY;
  }
  if (image == "bewolkt") {
    return MDI_WEATHER_CLOUDY;
  }
  if (image == "zwaarbewolkt") {
    return MDI_WEATHER_CLOUDY;
  }
  if (image == "lichtbewolkt") {
    return MDI_WEATHER_PARTLY_CLOUDY;
  }
  if (image == "halfbewolkt") {
    return MDI_WEATHER_PARTLY_CLOUDY;
  }
  if (image == "halfbewolkt_regen") {
    return MDI_WEATHER_PARTLY_RAINY;
  }
  if (image == "nachtmist") {
    return MDI_WEATHER_NIGHT;
  }
  if (image == "helderenacht") {
    return MDI_WEATHER_NIGHT;
  }
  if (image == "nachtbewolkt") {
    return MDI_WEATHER_NIGHT_PARTLY_CLOUDY;
  }
  return nullptr;
}
